using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text.Json;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Options;
using System.Management;

namespace DeployTool.Agent.Services;

/// <summary>
/// 프로세스 실행, 종료 및 목록 포함 프로세스 관리 작업을 제공합니다.
/// </summary>
public class ProcessService
{
	private readonly AgentConfig _cfg;
	private List<MonitoredProcessConfig> _watchlist = new();
	// uid → 이 서비스가 직접 시작한 PID 집합 (프로세스 재시작/종료 시 갱신)
	private readonly Dictionary<string, HashSet<int>> _managedPids = new();

	/// <summary>
	/// ProcessService의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="cfg">에이전트 설정</param>
	public ProcessService(IOptions<AgentConfig> cfg)
	{
		_cfg = cfg.Value;
		LoadWatchlist();
		ScanExistingProcesses();
	}

	/// <summary>
	/// Agent 시작 시 이미 실행 중인 감시 대상 프로세스를 스캔하여 _managedPids에 등록합니다.
	/// Agent가 재시작되어도 기존에 떠 있던 프로세스를 Running으로 인식할 수 있습니다.
	/// </summary>
	private void ScanExistingProcesses()
	{
		try
		{
			var allProcesses = Process.GetProcesses();
			foreach (var config in _watchlist)
			{
				if (string.IsNullOrWhiteSpace(config.Uid))
					continue;

				foreach (var p in allProcesses)
				{
					try
					{
						if (p.ProcessName.Equals(config.Name, StringComparison.OrdinalIgnoreCase) &&
							ProcessArgumentsMatch(p.Id, config.Arguments))
						{
							if (!_managedPids.ContainsKey(config.Uid))
								_managedPids[config.Uid] = new HashSet<int>();
							_managedPids[config.Uid].Add(p.Id);
						}
					}
					catch { }
				}
			}
		}
		catch (Exception ex)
		{
			System.Diagnostics.Debug.WriteLine($"ScanExistingProcesses failed: {ex.Message}");
		}
	}

	/// <summary>
	/// process-watchlist.json 파일에서 감시 프로세스 목록을 로드합니다.
	/// 기존 프로세스에 UID가 없으면 자동으로 생성합니다.
	/// </summary>
	private void LoadWatchlist()
	{
		try
		{
			var path = Path.Combine(AppContext.BaseDirectory, _cfg.ProcessWatchlistPath);
			if (File.Exists(path))
			{
				var json = File.ReadAllText(path);
				
				// JsonSerializer 옵션: 기존 파일과의 호환성을 위해 null 필드 허용
				var options = new System.Text.Json.JsonSerializerOptions
				{
					PropertyNameCaseInsensitive = true, // 대소문자 상관없음
					PropertyNamingPolicy = System.Text.Json.JsonNamingPolicy.CamelCase
				};
				
				_watchlist = System.Text.Json.JsonSerializer.Deserialize<List<MonitoredProcessConfig>>(json, options) ?? new();
				
				// UID가 없는 기존 프로세스에 UUID 자동 생성
				bool needsSave = false;
				foreach (var proc in _watchlist)
				{
					if (string.IsNullOrWhiteSpace(proc.Uid))
					{
						proc.Uid = Guid.NewGuid().ToString("N");
						needsSave = true;
					}
				}
				
				if (needsSave)
					SaveWatchlist();
			}
		}
		catch (Exception ex)
		{
			// 파싱 실패 시 로그 (필요시 Logger에 기록)
			System.Diagnostics.Debug.WriteLine($"Failed to load watchlist: {ex.Message}");
		}
	}
	/// <summary>
	/// 선택적 분리 및 출력 캡처를 통해 외부 프로세스를 실행합니다.
	/// </summary>
	/// <param name="req">프로그램 경로, 인수, 작업 디렉터리 및 분리 플래그를 포함하는 요청</param>
	/// <returns>프로세스 ID 및 종료 코드가 있는 결과 응답</returns>
	public async Task<ResultResponse> ExecuteAsync(ExecuteRequest req)
	{
		var psi = new ProcessStartInfo
		{
			FileName               = req.Program,
			WorkingDirectory       = req.WorkDir,
			UseShellExecute        = req.Detach,
			CreateNoWindow         = !req.Detach,
			RedirectStandardOutput = !req.Detach,
			RedirectStandardError  = !req.Detach,
		};
		foreach (var arg in req.Args)
			psi.ArgumentList.Add(arg);

		var proc = Process.Start(psi);
		if (null == proc)
			return Fail("Failed to start process");

		if (!req.Detach)
			await proc.WaitForExitAsync();

		return new ResultResponse
		{
			Success = true,
			Message = $"PID={proc.Id}",
			Code    = req.Detach ? proc.Id : proc.ExitCode
		};
	}

	/// <summary>
	/// PID별 프로세스를 종료하며 프로세스 트리의 모든 자식 프로세스를 포함합니다.
	/// </summary>
	/// <param name="req">프로세스 ID를 포함하는 요청</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> KillAsync(KillRequest req)
	{
		try
		{
			var proc = Process.GetProcessById(req.Pid);
			proc.Kill(entireProcessTree: true);
			return Task.FromResult(Ok());
		}
		catch (Exception ex)
		{
			return Task.FromResult(Fail(ex.Message));
		}
	}

	/// <summary>
	/// PID, 이름 및 메모리 사용량과 함께 모든 실행 프로세스를 나열합니다.
	/// </summary>
	/// <returns>프로세스 목록을 포함하는 응답</returns>
	public Task<ProcessListResponse> ListProcessAsync()
	{
		var list = new List<ProcessEntry>();
		var procs = Process.GetProcesses();
		
		foreach (var p in procs)
		{
			try
			{
				float mem = 0;
				try { mem = (float)(p.WorkingSet64 / 1024.0 / 1024.0); } catch { }
				list.Add(new ProcessEntry { Pid = p.Id, Name = p.ProcessName, MemMb = mem });
			}
			catch { }
		}

		return Task.FromResult(new ProcessListResponse { Processes = list });
	}

	/// <summary>
	/// 설정된 감시 대상 프로세스의 현재 상태를 조회합니다.
	/// </summary>
	/// <returns>감시 프로세스 상태 목록</returns>
	public Task<MonitoredProcessesResponse> GetMonitoredProcessesAsync()
	{
		var result = new List<MonitoredProcessStatus>();
		var allProcesses = Process.GetProcesses();

		foreach (var config in _watchlist)
		{
			// 1) 이 서비스가 직접 시작한 PID 중 아직 살아있는 것
			var livePids = new HashSet<int>();
			lock (_managedPids)
			{
				if (_managedPids.TryGetValue(config.Uid, out var mpids))
				{
					var dead = new List<int>();
					foreach (var pid in mpids)
					{
						try
						{
							var p2 = Process.GetProcessById(pid);
							if (!p2.HasExited)
								livePids.Add(pid);
							else
								dead.Add(pid);
						}
						catch { dead.Add(pid); }
					}
					foreach (var pid in dead) mpids.Remove(pid);
				}
			}

			// 2) 외부에서 실행된 프로세스 — 이름 + 인자로 스캔
			foreach (var p in allProcesses)
			{
				if (p.ProcessName.Equals(config.Name, StringComparison.OrdinalIgnoreCase) &&
					ProcessArgumentsMatch(p.Id, config.Arguments))
				{
					livePids.Add(p.Id);
				}
			}

			var matchingProcesses = allProcesses
				.Where(p => livePids.Contains(p.Id))
				.ToList();

			var status = new MonitoredProcessStatus
			{
				Uid = config.Uid,
				Name = config.Name,
				DisplayName = config.DisplayName,
				Description = config.Description,
				Path = config.Path,
				Priority = config.Priority,
				Arguments = config.Arguments,
				Status = matchingProcesses.Count > 0 ? "Running" : "Stopped",
				Pids = matchingProcesses.Select(p => p.Id).ToList(),
				LastCheckUtc = DateTime.UtcNow
			};

			// CPU와 메모리 합계 계산
			foreach (var proc in matchingProcesses)
			{
				try
				{
					status.TotalMemMb += (float)(proc.WorkingSet64 / 1024.0 / 1024.0);
				}
				catch { }
			}

			result.Add(status);
		}

		return Task.FromResult(new MonitoredProcessesResponse
		{
				Processes = result,
			CollectedAtUtc = DateTime.UtcNow
		});
	}

	/// <summary>
	/// 감시 프로세스 설정을 추가하거나 수정합니다.
	/// </summary>
	/// <param name="req">추가/수정할 프로세스 설정</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> UpsertMonitoredProcessAsync(UpsertMonitoredProcessRequest req)
	{
		try
		{
			if (string.IsNullOrWhiteSpace(req.Config.Name))
				return Task.FromResult(Fail("Process name is required"));

			if (string.IsNullOrWhiteSpace(req.Config.Path))
				return Task.FromResult(Fail("Process path is required"));

			// 기존 항목 제거 (수정하는 경우)
			if (!string.IsNullOrWhiteSpace(req.OldName))
				_watchlist.RemoveAll(p => p.Name.Equals(req.OldName, StringComparison.OrdinalIgnoreCase));

			// 새 항목 추가
			_watchlist.RemoveAll(p => p.Name.Equals(req.Config.Name, StringComparison.OrdinalIgnoreCase));
			
			// 신규 항목이면 UID 생성
			if (string.IsNullOrWhiteSpace(req.Config.Uid))
				req.Config.Uid = Guid.NewGuid().ToString("N");
			
			_watchlist.Add(req.Config);

			SaveWatchlist();
			return Task.FromResult(Ok());
		}
		catch (Exception ex)
		{
			return Task.FromResult(Fail(ex.Message));
		}
	}

	/// <summary>
	/// 감시 프로세스 설정을 삭제합니다.
	/// </summary>
	/// <param name="req">삭제할 프로세스 이름</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> DeleteMonitoredProcessAsync(DeleteMonitoredProcessRequest req)
	{
		try
		{
			if (string.IsNullOrWhiteSpace(req.Name))
				return Task.FromResult(Fail("Process name is required"));

			var removed = _watchlist.RemoveAll(p => p.Name.Equals(req.Name, StringComparison.OrdinalIgnoreCase));
			if (removed == 0)
				return Task.FromResult(Fail("Process not found in watchlist"));

			SaveWatchlist();
			return Task.FromResult(Ok());
		}
		catch (Exception ex)
		{
			return Task.FromResult(Fail(ex.Message));
		}
	}

	/// <summary>
	/// 감시 중인 프로세스를 시작합니다.
	/// </summary>
	/// <param name="req">시작할 프로세스의 UID</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> StartMonitoredProcessAsync(StartMonitoredProcessRequest req)
	{
		try
		{
			if (string.IsNullOrWhiteSpace(req.Uid))
				return Task.FromResult(Fail("Process UID is required"));

			var config = _watchlist.FirstOrDefault(p => p.Uid.Equals(req.Uid, StringComparison.OrdinalIgnoreCase));
			if (null == config)
				return Task.FromResult(Fail("Process not found in watchlist"));

			if (string.IsNullOrWhiteSpace(config.Path))
				return Task.FromResult(Fail("Process path is not configured"));

			// 이미 실행 중인지 확인 (프로세스 이름 + 명령줄 인자 모두 비교)
			var allProcesses = Process.GetProcesses();
			var matchingProcesses = allProcesses
				.Where(p => p.ProcessName.Equals(config.Name, StringComparison.OrdinalIgnoreCase) &&
						   ProcessArgumentsMatch(p.Id, config.Arguments))
				.ToList();
			
			if (matchingProcesses.Count > 0)
				return Task.FromResult(new ResultResponse { Success = true, Message = "Process is already running" });

			// 프로세스 시작
			var psi = new ProcessStartInfo
			{
				FileName         = config.Path,
				UseShellExecute  = true,
				CreateNoWindow   = false,
			};

			foreach (var arg in config.Arguments)
				psi.ArgumentList.Add(arg);

			var proc = Process.Start(psi);
			if (null == proc)
				return Task.FromResult(Fail("Failed to start process"));

			// 직접 시작한 PID 기록
			lock (_managedPids)
			{
				if (!_managedPids.ContainsKey(config.Uid))
					_managedPids[config.Uid] = new HashSet<int>();
				_managedPids[config.Uid].Add(proc.Id);
			}

			return Task.FromResult(Ok());
		}
		catch (Exception ex)
		{
			return Task.FromResult(Fail(ex.Message));
		}
	}

	/// <summary>
	/// 감시 중인 프로세스를 중지합니다.
	/// </summary>
	/// <param name="req">중지할 프로세스의 UID</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> StopMonitoredProcessAsync(StopMonitoredProcessRequest req)
	{
		try
		{
			if (string.IsNullOrWhiteSpace(req.Uid))
				return Task.FromResult(Fail("Process UID is required"));

			var config = _watchlist.FirstOrDefault(p => p.Uid.Equals(req.Uid, StringComparison.OrdinalIgnoreCase));
			if (null == config)
				return Task.FromResult(Fail("Process not found in watchlist"));

			// 일치하는 프로세스 종료 (이름 + 인자) 및 tracked PID도 함께 종료
			var allProcesses = Process.GetProcesses();
			var toKill = new HashSet<int>();

			// tracked PID
			lock (_managedPids)
			{
				if (_managedPids.TryGetValue(config.Uid, out var mpids))
				{
					foreach (var pid in mpids) toKill.Add(pid);
					_managedPids.Remove(config.Uid);
				}
			}

			// 이름+인자 스캔
			foreach (var proc in allProcesses)
			{
				if (proc.ProcessName.Equals(config.Name, StringComparison.OrdinalIgnoreCase) &&
					ProcessArgumentsMatch(proc.Id, config.Arguments))
				{
					toKill.Add(proc.Id);
				}
			}

			var stoppedCount = 0;
			foreach (var pid in toKill)
			{
				try
				{
					Process.GetProcessById(pid).Kill(entireProcessTree: true);
					stoppedCount++;
				}
				catch { }
			}

			if (stoppedCount == 0)
				return Task.FromResult(Fail("No matching processes found to stop"));

			return Task.FromResult(Ok());
		}
		catch (Exception ex)
		{
			return Task.FromResult(Fail(ex.Message));
		}
	}

	/// <summary>
	/// 감시 중인 프로세스를 재시작합니다.
	/// 설정된 경로와 Arguments를 사용하여 프로세스를 시작합니다.
	/// </summary>
	/// <param name="req">재시작할 프로세스 이름</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> RestartMonitoredProcessAsync(RestartMonitoredProcessRequest req)
	{
		try
		{
			if (string.IsNullOrWhiteSpace(req.Name))
				return Task.FromResult(Fail("Process name is required"));

			var config = _watchlist.FirstOrDefault(p => p.Name.Equals(req.Name, StringComparison.OrdinalIgnoreCase));
			if (null == config)
				return Task.FromResult(Fail("Process not found in watchlist"));

			if (string.IsNullOrWhiteSpace(config.Path))
				return Task.FromResult(Fail("Process path is not configured"));

			// 기존 프로세스 종료
			var allProcesses = Process.GetProcesses();
			foreach (var proc in allProcesses)
			{
				try
				{
					if (proc.ProcessName.Equals(config.Name, StringComparison.OrdinalIgnoreCase))
						proc.Kill(entireProcessTree: true);
				}
				catch { }
			}

			// 새 프로세스 시작
			var psi = new ProcessStartInfo
			{
				FileName         = config.Path,
				UseShellExecute  = true,
				CreateNoWindow   = false,
			};

			foreach (var arg in config.Arguments)
				psi.ArgumentList.Add(arg);

			var proc_new = Process.Start(psi);
			if (null == proc_new)
				return Task.FromResult(Fail("Failed to start process"));

			return Task.FromResult(Ok());
		}
		catch (Exception ex)
		{
			return Task.FromResult(Fail(ex.Message));
		}
	}

	/// <summary>
	/// watchlist를 파일에 저장합니다.
	/// </summary>
	private void SaveWatchlist()
	{
		try
		{
			var path = Path.Combine(AppContext.BaseDirectory, _cfg.ProcessWatchlistPath);
			var options = new System.Text.Json.JsonSerializerOptions 
			{ 
				WriteIndented = true,
				PropertyNamingPolicy = System.Text.Json.JsonNamingPolicy.CamelCase
			};
			var json = System.Text.Json.JsonSerializer.Serialize(_watchlist, options);
			File.WriteAllText(path, json);
		}
		catch (Exception ex)
		{
			System.Diagnostics.Debug.WriteLine($"Failed to save watchlist: {ex.Message}");
		}
	}

	private static ResultResponse Ok()                 => new() { Success = true };
	private static ResultResponse Fail(string message) => new() { Success = false, Message = message, Code = 400 };

	/// <summary>
	/// 실행 중인 프로세스의 명령줄 인자 목록을 가져옵니다. (크로스플랫폼)
	/// 첫 번째 토큰(실행 파일 경로)은 제외하고 인자만 반환합니다.
	/// </summary>
	private static List<string> GetProcessArguments(int pid)
	{
		try
		{
			if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
			{
				// Windows: WMI로 CommandLine 조회 후 파싱
				using var searcher = new ManagementObjectSearcher(
					$"SELECT CommandLine FROM Win32_Process WHERE ProcessId = {pid}");
				foreach (ManagementObject obj in searcher.Get())
				{
					var cmdLine = obj["CommandLine"]?.ToString();
					if (string.IsNullOrEmpty(cmdLine))
						return new();
					return ParseCommandLineArgs(cmdLine);
				}
			}
			else
			{
				// Linux: /proc/{pid}/cmdline 읽기 (null 구분자)
				var cmdlinePath = $"/proc/{pid}/cmdline";
				if (File.Exists(cmdlinePath))
				{
					var content = File.ReadAllBytes(cmdlinePath);
					// null 바이트로 구분된 인자 배열
					var tokens = System.Text.Encoding.UTF8.GetString(content)
						.TrimEnd('\0')
						.Split('\0');
					// 첫 번째 토큰은 실행 파일 경로
					return tokens.Skip(1).Where(t => !string.IsNullOrEmpty(t)).ToList();
				}
			}
		}
		catch { }
		return new();
	}

	/// <summary>
	/// Windows 명령줄 문자열을 토큰 목록으로 파싱합니다.
	/// 따옴표로 감싸진 경로/인자를 올바르게 처리합니다.
	/// 반환값은 첫 번째 토큰(실행 파일)을 제외한 인자 목록입니다.
	/// </summary>
	private static List<string> ParseCommandLineArgs(string cmdLine)
	{
		var args = new List<string>();
		var token = new System.Text.StringBuilder();
		bool inQuote = false;
		bool isFirst = true;

		for (int i = 0; i < cmdLine.Length; i++)
		{
			char c = cmdLine[i];

			if (c == '"')
			{
				// 이중 따옴표 escape ("")
				if (inQuote && i + 1 < cmdLine.Length && cmdLine[i + 1] == '"')
				{
					token.Append('"');
					i++;
				}
				else
				{
					inQuote = !inQuote;
				}
			}
			else if (c == ' ' && !inQuote)
			{
				if (token.Length > 0)
				{
					if (isFirst)
						isFirst = false; // 첫 번째 토큰(exe 경로)은 건너뜀
					else
						args.Add(token.ToString());
					token.Clear();
				}
			}
			else
			{
				token.Append(c);
			}
		}

		if (token.Length > 0 && !isFirst)
			args.Add(token.ToString());

		return args;
	}

	/// <summary>
	/// 프로세스의 명령줄 인자가 설정과 일치하는지 확인합니다. (크로스플랫폼)
	/// expectedArgs가 비어있으면 모든 프로세스와 일치합니다.
	/// </summary>
	private static bool ProcessArgumentsMatch(int pid, List<string> expectedArgs)
	{
		if (expectedArgs == null || expectedArgs.Count == 0)
			return true;

		var actualArgs = GetProcessArguments(pid);

		if (actualArgs.Count != expectedArgs.Count)
			return false;

		for (int i = 0; i < expectedArgs.Count; i++)
		{
			if (!actualArgs[i].Equals(expectedArgs[i], StringComparison.OrdinalIgnoreCase))
				return false;
		}

		return true;
	}
}
