namespace DeployTool.Agent.Services;

/// <summary>
/// appsettings.json에서 로드된 에이전트 설정
/// </summary>
public class AgentConfig
{
	/// <summary>
	/// TCP 수신 포트. 기본값: 7700
	/// </summary>
	public int ListenPort { get; set; } = 7700;

	/// <summary>
	/// 파일 작업의 기본 작업 디렉터리
	/// </summary>
	public string WorkDir { get; set; } = ".";

	/// <summary>
	/// 로그 파일 조회를 위한 디렉터리
	/// </summary>
	public string LogDir { get; set; } = ".";

	/// <summary>
	/// 패치 파일의 루트 디렉터리. 비어 있으면 WorkDir 사용
	/// </summary>
	public string PatchRootDir { get; set; } = "";

	/// <summary>
	/// 셸 명령 실행 활성화 (위험함)
	/// </summary>
	public bool AllowShell { get; set; } = false;

	/// <summary>
	/// 최대 파일 크기(바이트). 기본값: 100MB
	/// </summary>
	public long MaxFileSizeBytes { get; set; } = 104_857_600;

	/// <summary>
	/// 매니저 연결을 위한 IP 화이트리스트. 빈 목록은 모든 IP 허용
	/// </summary>
	public List<string> AllowedManagerHosts { get; set; } = new();

	/// <summary>
	/// 외부 데이터베이스 및 캐시 서비스 설정 (MySQL, MSSQL, Redis 등)
	/// </summary>
	public Dictionary<string, ServiceConnectionConfig> Services { get; set; } = new();

	/// <summary>
	/// 감시할 프로세스 목록 설정 파일 경로 (process-watchlist.json)
	/// </summary>
	public string ProcessWatchlistPath { get; set; } = "process-watchlist.json";
}

/// <summary>
/// 외부 서비스 연결 설정
/// </summary>
public class ServiceConnectionConfig
{
	/// <summary>서비스 호스트 주소 또는 IP</summary>
	public string Host { get; set; } = "";
	/// <summary>서비스 포트 번호</summary>
	public int Port { get; set; }
	/// <summary>데이터베이스 이름 (SQL 서비스용)</summary>
	public string? Database { get; set; }
	/// <summary>인증을 위한 사용자명</summary>
	public string? Username { get; set; }
	/// <summary>인증을 위한 암호</summary>
	public string? Password { get; set; }
}
