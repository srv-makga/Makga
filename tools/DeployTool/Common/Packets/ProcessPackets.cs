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
