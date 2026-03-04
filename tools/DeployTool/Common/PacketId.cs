namespace DeployTool.Common;

/// <summary>
/// 매니저와 에이전트 간 통신을 위한 패킷 ID를 정의합니다.
/// 값 1-99는 매니저 → 에이전트 요청이고 101+는 에이전트 → 매니저 응답입니다.
/// </summary>
public enum PacketId : ushort
{
	None = 0,

	// ── 매니저 → 에이전트 (요청) ────────────────────────
	/// <summary>토큰을 사용한 연결 인증</summary>
	Ping         = 1,
	/// <summary>에이전트 정보 요청</summary>
	GetInfo      = 2,

	// 파일 작업 (10-29)
	/// <summary>작업 디렉터리 변경</summary>
	SetWorkDir   = 10,
	/// <summary>파일 및 디렉터리 나열</summary>
	ListFiles    = 11,
	/// <summary>파일 내용 다운로드</summary>
	GetFile      = 12,
	/// <summary>파일 업로드 (패치 배포)</summary>
	PutFile      = 13,
	/// <summary>파일 또는 디렉터리 삭제</summary>
	DeleteFile   = 14,
	/// <summary>파일/디렉터리 이동 또는 이름 변경</summary>
	MoveFile     = 15,
	/// <summary>디렉터리 생성</summary>
	MakeDir      = 16,
	/// <summary>파일 SHA-256 해시 가져오기</summary>
	GetFileHash  = 17,
	/// <summary>로컬 및 원격 파일 해시 비교</summary>
	SyncDir      = 18,
	/// <summary>디렉터리의 압축 백업 생성</summary>
	BackupDir    = 19,

	// 압축 (20-29)
	/// <summary>디렉터리를 ZIP 또는 TAR.GZ로 압축</summary>
	Compress     = 20,
	/// <summary>ZIP 또는 TAR.GZ 아카이브 압축 해제</summary>
	Decompress   = 21,
	/// <summary>청크 단위 파일 업로드 (패치 배포용)</summary>
	UploadChunk  = 22,

	// 프로세스 작업 (30-39)
	/// <summary>외부 프로세스 실행</summary>
	Execute      = 30,
	/// <summary>PID별 프로세스 종료</summary>
	Kill         = 31,
	/// <summary>실행 중인 프로세스 나열</summary>
	ListProcess  = 32,
	/// <summary>감시 중인 프로세스 상태 조회</summary>
	GetMonitoredProcesses = 33,
	/// <summary>감시 프로세스 추가/수정</summary>
	UpsertMonitoredProcess = 34,
	/// <summary>감시 프로세스 삭제</summary>
	DeleteMonitoredProcess = 35,
	/// <summary>감시 프로세스 재시작</summary>
	RestartMonitoredProcess = 36,
	/// <summary>감시 프로세스 시작</summary>
	StartMonitoredProcess = 37,
	/// <summary>감시 프로세스 중지</summary>
	StopMonitoredProcess = 38,

	// 서비스 작업 (40-49)
	/// <summary>Windows/systemd 서비스 시작</summary>
	ServiceStart  = 40,
	/// <summary>Windows/systemd 서비스 중지</summary>
	ServiceStop   = 41,
	/// <summary>서비스 상태 가져오기</summary>
	ServiceStatus = 42,
	/// <summary>사용 가능한 서비스 나열</summary>
	ServiceList   = 43,

	// 리소스 정보 (50-59)
	/// <summary>CPU/메모리/디스크 정보 가져오기</summary>
	GetResource  = 50,
	/// <summary>네트워크 인터페이스 통계 가져오기</summary>
	GetNetwork   = 51,
	/// <summary>포트가 수신 대기 중인지 확인</summary>
	CheckPort    = 52,
	/// <summary>외부 서비스 상태 가져오기 (MySQL, MSSQL, Redis 등)</summary>
	GetExternalServices = 53,

	// 로그 작업 (60-69)
	/// <summary>로그 파일의 마지막 N개 라인 가져오기</summary>
	GetLog       = 60,
	/// <summary>로그 스트리밍 시작</summary>
	WatchLog     = 61,
	/// <summary>로그 스트리밍 중지</summary>
	StopWatchLog = 62,

	// 셸 작업 (70-79)
	/// <summary>셸 명령 실행</summary>
	Shell        = 70,

	// 설정 (80-89)
	/// <summary>에이전트 설정 가져오기 (appsettings.json)</summary>
	GetConfig    = 80,
	/// <summary>에이전트 설정 업데이트</summary>
	SetConfig    = 81,
	/// <summary>디스크에서 에이전트 설정 다시 로드</summary>
	ReloadConfig = 82,

	// ── 에이전트 → 매니저 (응답) ────────────────────────
	/// <summary>가동 시간이 있는 Ping 응답</summary>
	Pong         = 101,
	/// <summary>에이전트 정보 응답</summary>
	AgentInfo    = 102,
	/// <summary>일반 성공/실패 응답</summary>
	Result       = 103,

	// 파일 작업 응답 (111-119)
	/// <summary>파일 목록 응답</summary>
	FileList     = 111,
	/// <summary>파일 내용 응답 (이진 Base64)</summary>
	FileData     = 112,
	/// <summary>파일 해시 응답</summary>
	FileHash     = 113,
	/// <summary>동기화 비교 결과</summary>
	SyncDiff     = 114,

	// 프로세스 응답 (121-122)
	/// <summary>프로세스 목록 응답</summary>
	ProcessList   = 121,
	/// <summary>감시 중인 프로세스 상태 응답</summary>
	MonitoredProcesses = 122,

	// 서비스 응답 (131-132)
	/// <summary>서비스 작업 결과</summary>
	ServiceResult = 131,
	/// <summary>서비스 목록 응답</summary>
	ServiceItems  = 132,

	// 리소스 정보 응답 (141-144)
	/// <summary>리소스 정보 응답</summary>
	ResourceInfo = 141,
	/// <summary>네트워크 정보 응답</summary>
	NetworkInfo  = 142,
	/// <summary>포트 상태 응답</summary>
	PortStatus   = 143,
	/// <summary>외부 서비스 상태 응답</summary>
	ExternalServicesInfo = 144,

	// 로그 응답 (151)
	/// <summary>로그 데이터 응답 (tail 또는 스트리밍 청크)</summary>
	LogData      = 151,

	// 셸 응답 (161)
	/// <summary>셸 명령 출력 응답</summary>
	ShellOutput  = 161,

	// 설정 응답 (171)
	/// <summary>설정 데이터 응답</summary>
	ConfigData   = 171,
}
