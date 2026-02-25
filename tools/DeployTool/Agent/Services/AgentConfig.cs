namespace DeployTool.Agent.Services;

public class AgentConfig
{
	public int          ListenPort          { get; set; } = 7700;
	public string       Token               { get; set; } = "";
	public string       WorkDir             { get; set; } = ".";
	public string       LogDir              { get; set; } = ".";
	public bool         AllowShell          { get; set; } = false;
	public long         MaxFileSizeBytes    { get; set; } = 104_857_600; // 100MB
	public List<string> AllowedManagerHosts { get; set; } = new();
}
