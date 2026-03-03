using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text.Json;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Options;

namespace DeployTool.Agent.Services;

/// <summary>
/// 프로세스 실행, 종료 및 목록 포함 프로세스 관리 작업을 제공합니다.
/// </summary>
public class ProcessService
{
	private readonly AgentConfig _cfg;
	private List<MonitoredProcessConfig> _watchlist = new();

	/// <summary>
	/// ProcessService의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="cfg">에이전트 설정</param>
	public ProcessService(IOptions<AgentConfig> cfg)
	{
		_cfg = cfg.Value;
		LoadWatchlist();
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
			var matchingProcesses = allProcesses
				.Where(p => p.ProcessName.Equals(config.Name, StringComparison.OrdinalIgnoreCase))
				.ToList();

			var status = new MonitoredProcessStatus
			{
				Uid = config.Uid,
				Name = config.Name,
				DisplayName = config.DisplayName,
				Description = config.Description,
				Path = config.Path,
				Priority = config.Priority,
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

			// 이미 실행 중인지 확인
			var allProcesses = Process.GetProcesses();
			if (allProcesses.Any(p => p.ProcessName.Equals(config.Name, StringComparison.OrdinalIgnoreCase)))
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

			// 모든 일치하는 프로세스 종료
			var allProcesses = Process.GetProcesses();
			var stoppedCount = 0;
			foreach (var proc in allProcesses)
			{
				try
				{
					if (proc.ProcessName.Equals(config.Name, StringComparison.OrdinalIgnoreCase))
					{
						proc.Kill(entireProcessTree: true);
						stoppedCount++;
					}
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
}
