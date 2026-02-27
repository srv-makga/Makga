namespace DeployTool.Common.Packets;

// ── Manager → Agent ───────────────────────────────────────────
public class GetLogRequest
{
	public string Path  { get; set; } = "";
	public int    Lines { get; set; } = 100; // tail N lines
}

public class WatchLogRequest   { public string Path { get; set; } = ""; }
public class StopWatchLogRequest { }

public class ShellRequest
{
	public string       Command { get; set; } = "";
	public List<string> Args    { get; set; } = new();
	public string       WorkDir { get; set; } = "";
}

// ── Agent → Manager ───────────────────────────────────────────
public class LogDataResponse
{
	public string Path    { get; set; } = "";
	public string Content { get; set; } = "";
	public bool   IsStream { get; set; } = false; // true = 실시간 스트리밍 청크
}

public class ShellOutputResponse
{
	public string Stdout   { get; set; } = "";
	public string Stderr   { get; set; } = "";
	public int    ExitCode { get; set; }
}

// ── 설정 ──────────────────────────────────────────────────────
public class SetConfigRequest
{
	// key: "DeployAgent:WorkDir", value: "/new/path" 형태로 전달
	public Dictionary<string, string> Changes { get; set; } = new();
}

public class ConfigDataResponse
{
	public string Json { get; set; } = ""; // appsettings.json 전체 내용
}
