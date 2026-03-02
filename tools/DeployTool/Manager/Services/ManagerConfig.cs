namespace DeployTool.Manager.Services;

/// <summary>연결 및 하트비트 매개변수를 위한 매니저 설정 옵션</summary>
public class ManagerConfig
{
	/// <summary>serverlist.json 파일의 경로</summary>
	public string ServerListPath       { get; set; } = "serverlist.json";
	/// <summary>HTTP 서버 수신 포트</summary>
	public int    ListenPort           { get; set; } = 5000;
	/// <summary>실패한 에이전트 연결을 다시 시도하기 전에 기다릴 초</summary>
	public int    RetryIntervalSec     { get; set; } = 10;
	/// <summary>하트비트/리소스 모니터링 요청 간 초</summary>
	public int    HeartbeatIntervalSec { get; set; } = 5;
	/// <summary>업로드된 패치 파일을 저장하기 위한 디렉터리 경로</summary>
	public string UploadDir            { get; set; } = "Upload";
}
