namespace DeployTool.Manager.Services;

public class ManagerConfig
{
	public string ServerListPath       { get; set; } = "serverlist.json";
	public int    ListenPort           { get; set; } = 5000;
	public int    RetryIntervalSec     { get; set; } = 10;
	public int    HeartbeatIntervalSec { get; set; } = 5;
	public string UploadDir            { get; set; } = "Upload"; // 업로드 파일 저장 경로
}
