namespace DeployTool.Common.Packets;

/// <summary>외부 프로세스를 실행하기 위한 요청</summary>
public class ExecuteRequest
{
	/// <summary>실행할 프로그램 경로 또는 이름</summary>
	public string       Program { get; set; } = "";
	/// <summary>명령줄 인수</summary>
	public List<string> Args    { get; set; } = new();
	/// <summary>프로세스의 작업 디렉터리</summary>
	public string       WorkDir { get; set; } = "";
	/// <summary>true로 프로세스를 분리 (출력 캡처 없음), false로 완료를 대기</summary>
	public bool         Detach  { get; set; } = false;
}

/// <summary>PID별 프로세스를 종료하기 위한 요청</summary>
public class KillRequest        { 
	/// <summary>종료할 프로세스 ID</summary>
	public int Pid { get; set; } 
}

/// <summary>Windows 서비스 또는 systemd 서비스를 관리하기 위한 요청</summary>
public class ServiceNameRequest { 
	/// <summary>서비스 이름</summary>
	public string Name { get; set; } = ""; 
}

// ── 에이전트 → 매니저 ───────────────────────────────────────────
/// <summary>리소스 사용량이 있는 프로세스 정보</summary>
public class ProcessEntry
{
	/// <summary>프로세스 ID</summary>
	public int    Pid    { get; set; }
	/// <summary>프로세스 이름</summary>
	public string Name   { get; set; } = "";
	/// <summary>CPU 사용률 백분율</summary>
	public float  CpuPct { get; set; }
	/// <summary>메모리 사용량(메가바이트)</summary>
	public float  MemMb  { get; set; }
}

/// <summary>실행 중인 프로세스 목록을 포함하는 응답</summary>
public class ProcessListResponse
{
	/// <summary>실행 중인 프로세스 목록</summary>
	public List<ProcessEntry> Processes { get; set; } = new();
}

/// <summary>이름, 표시 이름 및 상태를 포함한 서비스 메타데이터</summary>
public class ServiceEntry
{
	/// <summary>서비스 내부 이름</summary>
	public string Name        { get; set; } = "";
	/// <summary>서비스 표시 이름 (Windows만 해당)</summary>
	public string DisplayName { get; set; } = "";
	/// <summary>서비스 상태 (Running, Stopped 등)</summary>
	public string Status      { get; set; } = "";
}

/// <summary>상태가 있는 서비스 작업 결과</summary>
public class ServiceStatusResponse
{
	/// <summary>서비스 이름</summary>
	public string Name   { get; set; } = "";
	/// <summary>현재 서비스 상태</summary>
	public string Status { get; set; } = "";
}

/// <summary>사용 가능한 서비스 목록을 포함하는 응답</summary>
public class ServiceListResponse
{
	/// <summary>사용 가능한 서비스 목록</summary>
	public List<ServiceEntry> Services { get; set; } = new();
}

// ── 프로세스 모니터링 ────────────────────────────────────────────
/// <summary>감시할 프로세스 설정</summary>
public class MonitoredProcessConfig
{
	/// <summary>프로세스 고유 식별자 (UUID)</summary>
	public string Uid { get; set; } = "";
	/// <summary>감시할 프로세스 이름 (소문자, 확장자 제외: 예 "sqlserver", "mysql")</summary>
	public string Name { get; set; } = "";
	/// <summary>사용자에게 표시할 이름</summary>
	public string DisplayName { get; set; } = "";
	/// <summary>프로세스 설명</summary>
	public string? Description { get; set; }
	/// <summary>우선순위 (critical, high, medium, low)</summary>
	public string Priority { get; set; } = "medium";
	/// <summary>프로세스가 종료되면 자동 재시작 여부</summary>
	public bool AutoRestart { get; set; } = false;
	/// <summary>프로세스 실행 파일의 전체 경로</summary>
	public string Path { get; set; } = "";
	/// <summary>프로세스 시작 시 전달할 명령줄 인수</summary>
	public List<string> Arguments { get; set; } = new();
}

/// <summary>감시 중인 프로세스의 현재 상태</summary>
public class MonitoredProcessStatus
{
	/// <summary>프로세스 고유 식별자 (UUID)</summary>
	public string Uid { get; set; } = "";
	/// <summary>감시 프로세스 설정 이름</summary>
	public string Name { get; set; } = "";
	/// <summary>표시할 이름</summary>
	public string DisplayName { get; set; } = "";
	/// <summary>프로세스 설명</summary>
	public string? Description { get; set; }
	/// <summary>프로세스 실행 파일의 전체 경로</summary>
	public string Path { get; set; } = "";
	/// <summary>우선순위 (critical, high, medium, low)</summary>
	public string Priority { get; set; } = "medium";
	/// <summary>현재 상태 ("Running", "Stopped", "Unknown")</summary>
	public string Status { get; set; } = "Unknown";
	/// <summary>일치하는 프로세스의 PID 목록</summary>
	public List<int> Pids { get; set; } = new();
	/// <summary>프로세스 시작 시 전달할 명령줄 인수</summary>
	public List<string> Arguments { get; set; } = new();
	/// <summary>총 CPU 사용률 합계 (여러 프로세스인 경우)</summary>
	public float TotalCpuPct { get; set; }
	/// <summary>총 메모리 사용량(메가바이트)</summary>
	public float TotalMemMb { get; set; }
	/// <summary>마지막 확인 시간 (UTC)</summary>
	public DateTime LastCheckUtc { get; set; }
}

/// <summary>감시 중인 모든 프로세스의 상태 응답</summary>
public class MonitoredProcessesResponse
{
	/// <summary>감시 중인 프로세스 목록</summary>
	public List<MonitoredProcessStatus> Processes { get; set; } = new();
	/// <summary>데이터 수집 시간 (UTC)</summary>
	public DateTime CollectedAtUtc { get; set; }
}

/// <summary>감시 프로세스 추가/수정 요청</summary>
public class UpsertMonitoredProcessRequest
{
	/// <summary>기존 프로세스를 수정하는 경우 이전 이름 (신규 추가 시 null)</summary>
	public string? OldName { get; set; }
	/// <summary>프로세스 설정</summary>
	public MonitoredProcessConfig Config { get; set; } = new();
}

/// <summary>감시 프로세스 삭제 요청</summary>
public class DeleteMonitoredProcessRequest
{
	/// <summary>삭제할 프로세스 이름</summary>
	public string Name { get; set; } = "";
}

/// <summary>감시 프로세스 재시작 요청</summary>
public class RestartMonitoredProcessRequest
{
	/// <summary>재시작할 프로세스 이름</summary>
	public string Name { get; set; } = "";
}

/// <summary>감시 프로세스 시작 요청</summary>
public class StartMonitoredProcessRequest
{
	/// <summary>시작할 프로세스의 UUID</summary>
	public string Uid { get; set; } = "";
}

/// <summary>감시 프로세스 중지 요청</summary>
public class StopMonitoredProcessRequest
{
	/// <summary>중지할 프로세스의 UUID</summary>
	public string Uid { get; set; } = "";
}
