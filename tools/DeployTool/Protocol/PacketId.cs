namespace DeployTool.Protocol;

public enum PacketId : ushort
{
	None = 0,

	// ── Manager → Agent ────────────────────────
	Ping         = 1,   // 연결 확인 + 인증 토큰
	GetInfo      = 2,   // Agent 기본 정보 요청

	// 파일
	SetWorkDir   = 10,  // 작업 디렉토리 변경
	ListFiles    = 11,  // 파일/디렉토리 목록 조회
	GetFile      = 12,  // 파일 내려받기
	PutFile      = 13,  // 파일 업로드 (패치)
	DeleteFile   = 14,  // 파일/디렉토리 삭제
	MoveFile     = 15,  // 파일/디렉토리 이동·이름 변경
	MakeDir      = 16,  // 디렉토리 생성
	GetFileHash  = 17,  // 파일 해시(SHA-256) 조회
	SyncDir      = 18,  // 로컬↔원격 파일 해시 비교 (diff 목록 반환)
	BackupDir    = 19,  // 디렉토리를 압축 백업

	// 압축
	Compress     = 20,  // 압축
	Decompress   = 21,  // 압축 해제

	// 프로세스
	Execute      = 30,  // 프로세스 실행
	Kill         = 31,  // 프로세스 종료 (PID)
	ListProcess  = 32,  // 실행 중인 프로세스 목록

	// 서비스 (Windows Service / systemd)
	ServiceStart  = 40, // 서비스 시작
	ServiceStop   = 41, // 서비스 중지
	ServiceStatus = 42, // 서비스 상태 조회
	ServiceList   = 43, // 등록된 서비스 목록

	// 리소스
	GetResource  = 50,  // CPU/Memory/Disk 조회
	GetNetwork   = 51,  // 네트워크 인터페이스/트래픽 조회
	CheckPort    = 52,  // 특정 포트 Listen 여부 확인

	// 로그
	GetLog       = 60,  // 로그 파일 tail
	WatchLog     = 61,  // 로그 실시간 스트리밍 시작
	StopWatchLog = 62,  // 로그 스트리밍 중지

	// 셸
	Shell        = 70,  // 임의 셸 명령 실행 (stdout/stderr 반환)

	// 설정
	GetConfig    = 80,  // Agent 설정(appsettings.json) 조회
	SetConfig    = 81,  // Agent 설정 항목 수정 (부분 업데이트)
	ReloadConfig = 82,  // 설정 재로드 (디스크 재읽기)

	// ── Agent → Manager ────────────────────────
	Pong         = 101, // Ping 응답
	AgentInfo    = 102, // Agent 기본 정보
	Result       = 103, // 범용 성공/실패 응답

	FileList     = 111, // 파일 목록
	FileData     = 112, // 파일 내용 (바이너리 Base64)
	FileHash     = 113, // 파일 해시
	SyncDiff     = 114, // SyncDir diff 목록

	ProcessList   = 121, // 프로세스 목록
	ServiceResult = 131, // 서비스 상태
	ServiceItems  = 132, // 서비스 목록

	ResourceInfo = 141, // 리소스 정보
	NetworkInfo  = 142, // 네트워크 정보
	PortStatus   = 143, // 포트 Listen 여부

	LogData      = 151, // 로그 데이터 (tail / stream chunk)
	ShellOutput  = 161, // 셸 명령 결과

	ConfigData   = 171, // Agent 설정 내용
}
