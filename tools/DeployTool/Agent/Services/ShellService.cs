using System.Diagnostics;
using System.Runtime.InteropServices;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Options;

namespace DeployTool.Agent.Services;

public class ShellService
{
	private readonly AgentConfig _cfg;

	public ShellService(IOptions<AgentConfig> cfg) => _cfg = cfg.Value;

	// @brief 로그 파일의 마지막 N줄을 반환
	public async Task<LogDataResponse> GetLogAsync(GetLogRequest req)
	{
		var path = Path.IsPathRooted(req.Path)
			? req.Path
			: Path.Combine(_cfg.LogDir, req.Path);

		if (!File.Exists(path))
			return new LogDataResponse { Path = req.Path, Content = "" };

		var lines   = await File.ReadAllLinesAsync(path);
		var tail    = lines.TakeLast(req.Lines);
		return new LogDataResponse { Path = req.Path, Content = string.Join('\n', tail) };
	}

	// @brief 셸 명령 실행 (AllowShell 플래그 검사)
	public async Task<ShellOutputResponse> RunAsync(ShellRequest req)
	{
		if (!_cfg.AllowShell)
			return new ShellOutputResponse { Stderr = "Shell execution is disabled", ExitCode = -1 };

		var (shell, shellArg) = GetShell();

		// 명령과 인수를 하나의 문자열로 조합
		var cmdLine = req.Args.Count > 0
			? $"{req.Command} {string.Join(' ', req.Args)}"
			: req.Command;

		using var proc = new Process();
		proc.StartInfo = new ProcessStartInfo
		{
			FileName               = shell,
			ArgumentList           = { shellArg, cmdLine },
			WorkingDirectory       = req.WorkDir,
			RedirectStandardOutput = true,
			RedirectStandardError  = true,
			UseShellExecute        = false
		};

		proc.Start();
		var stdout = await proc.StandardOutput.ReadToEndAsync();
		var stderr = await proc.StandardError.ReadToEndAsync();
		await proc.WaitForExitAsync();

		return new ShellOutputResponse
		{
			Stdout   = stdout,
			Stderr   = stderr,
			ExitCode = proc.ExitCode
		};
	}

	private static (string shell, string arg) GetShell() =>
		RuntimeInformation.IsOSPlatform(OSPlatform.Windows)
			? ("cmd.exe", "/c")
			: ("/bin/sh", "-c");
}
