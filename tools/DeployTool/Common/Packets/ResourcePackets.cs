namespace DeployTool.Common.Packets;

/// <summary>포트 수신 상태를 확인하기 위한 요청</summary>
public class CheckPortRequest { 
	/// <summary>확인할 TCP 포트 번호</summary>
	public int Port { get; set; } 
}

// ── 에이전트 → 매니저 ───────────────────────────────────────────
/// <summary>CPU, 메모리 및 디스크를 포함한 시스템 리소스 정보</summary>
public class ResourceInfoResponse
{
	/// <summary>CPU 사용률 백분율</summary>
	public float  CpuPct      { get; set; }
	/// <summary>사용된 메모리(메가바이트)</summary>
	public long   MemUsedMb   { get; set; }
	/// <summary>사용 가능한 총 메모리(메가바이트)</summary>
	public long   MemTotalMb  { get; set; }
	/// <summary>사용된 디스크 공간(기가바이트)</summary>
	public double DiskUsedGb  { get; set; }
	/// <summary>총 디스크 용량(기가바이트)</summary>
	public double DiskTotalGb { get; set; }
	/// <summary>운영 체제 설명</summary>
	public string Os          { get; set; } = "";
}

/// <summary>네트워크 인터페이스 통계</summary>
public class NetworkInterfaceEntry
{
	/// <summary>네트워크 인터페이스 이름</summary>
	public string Name      { get; set; } = "";
	/// <summary>이 인터페이스에서 전송된 총 바이트</summary>
	public long   BytesSent { get; set; }
	/// <summary>이 인터페이스에서 수신된 총 바이트</summary>
	public long   BytesRecv { get; set; }
}

/// <summary>모든 어댑터의 네트워크 인터페이스 정보</summary>
public class NetworkInfoResponse
{
	/// <summary>트래픽 통계가 있는 네트워크 인터페이스 목록</summary>
	public List<NetworkInterfaceEntry> Interfaces { get; set; } = new();
}

/// <summary>포트 수신 상태</summary>
public class PortStatusResponse
{
	/// <summary>확인된 포트 번호</summary>
	public int  Port     { get; set; }
	/// <summary>포트가 수신 대기 중이면 true, 아니면 false</summary>
	public bool Listening { get; set; }
}
