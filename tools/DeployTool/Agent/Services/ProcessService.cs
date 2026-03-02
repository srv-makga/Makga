using System.Diagnostics;
using System.Runtime.InteropServices;
using DeployTool.Common.Packets;

namespace DeployTool.Agent.Services;

/// <summary>
/// 프로세스 실행, 종료 및 목록 포함 프로세스 관리 작업을 제공합니다.
/// </summary>
public class ProcessService
{
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

	private static ResultResponse Ok()                 => new() { Success = true };
	private static ResultResponse Fail(string message) => new() { Success = false, Message = message, Code = 400 };
}
