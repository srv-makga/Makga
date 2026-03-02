using System.Diagnostics;
using System.Runtime.InteropServices;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Options;

namespace DeployTool.Agent.Services;

/// <summary>
/// 셸 명령 실행 및 로그 파일 tail 작업을 제공합니다.
/// </summary>
public class ShellService
{
	private readonly AgentConfig _cfg;

	/// <summary>
	/// ShellService 클래스의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="cfg">에이전트 설정 옵션</param>
	public ShellService(IOptions<AgentConfig> cfg) => _cfg = cfg.Value;

	/// <summary>
	/// 로그 파일의 마지막 N개 라인을 검색합니다.
	/// </summary>
	/// <param name="req">로그 파일 경로 및 라인 수를 포함하는 요청</param>
	/// <returns>요청된 로그 라인을 포함하는 응답</returns>
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

	/// <summary>
	/// 셸 명령을 실행하고 stdout, stderr 및 종료 코드를 반환합니다.
	/// 셸 실행은 설정에서 AllowShell이 활성화된 경우에만 허용됩니다.
	/// </summary>
	/// <param name="req">명령, 인수 및 작업 디렉터리를 포함하는 요청</param>
	/// <returns>명령 출력 및 종료 코드를 포함하는 응답</returns>
	public async Task<ShellOutputResponse> RunAsync(ShellRequest req)
	{
		if (!_cfg.AllowShell)
			return new ShellOutputResponse { Stderr = "Shell execution is disabled", ExitCode = -1 };

		var (shell, shellArg) = GetShell();

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
		var stdoutTask = proc.StandardOutput.ReadToEndAsync();
		var stderrTask = proc.StandardError.ReadToEndAsync();
		await Task.WhenAll(stdoutTask, stderrTask);
		var stdout = stdoutTask.Result;
		var stderr = stderrTask.Result;

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
