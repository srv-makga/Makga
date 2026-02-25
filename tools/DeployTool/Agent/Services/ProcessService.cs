using System.Diagnostics;
using System.Runtime.InteropServices;
using DeployTool.Protocol.Packets;

namespace DeployTool.Agent.Services;

public class ProcessService
{
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

	public Task<ProcessListResponse> ListProcessAsync()
	{
		var list = Process.GetProcesses().Select(p =>
		{
			float mem = 0;
			try { mem = (float)(p.WorkingSet64 / 1024.0 / 1024.0); } catch { }
			return new ProcessEntry { Pid = p.Id, Name = p.ProcessName, MemMb = mem };
		}).ToList();

		return Task.FromResult(new ProcessListResponse { Processes = list });
	}

	private static ResultResponse Ok()                 => new() { Success = true };
	private static ResultResponse Fail(string message) => new() { Success = false, Message = message, Code = 400 };
}
