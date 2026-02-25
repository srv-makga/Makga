namespace DeployTool.Manager.Services;

public class ManagerConfig
{
	public string ServerListPath { get; set; } = "serverlist.json";
	public int    ListenPort     { get; set; } = 5000;
}
