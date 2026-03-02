namespace DeployTool.Common.Packets;

// ── 매니저 → 에이전트 ───────────────────────────────────────────
/// <summary>외부 서비스 상태를 쿼리하기 위한 요청</summary>
public class ExternalServicesInfoRequest
{
}

// ── 에이전트 → 매니저 ───────────────────────────────────────────
/// <summary>외부 서비스 설정 (연결 정보)</summary>
public class ExternalServiceConfig
{
	/// <summary>서비스 유형 (MySQL, MSSQL, Redis 등)</summary>
	public string ServiceType { get; set; } = "";
	/// <summary>호스트 주소 또는 IP</summary>
	public string Host { get; set; } = "";
	/// <summary>포트 번호</summary>
	public int Port { get; set; }
	/// <summary>데이터베이스 이름 (SQL 서비스용)</summary>
	public string? Database { get; set; }
	/// <summary>인증을 위한 사용자명</summary>
	public string? Username { get; set; }
}

/// <summary>외부 서비스 상태</summary>
public class ExternalServiceStatus
{
	/// <summary>서비스 유형 (MySQL, MSSQL, Redis)</summary>
	public string ServiceType { get; set; } = "";
	/// <summary>표시 이름 (예: "MySQL-prod")</summary>
	public string DisplayName { get; set; } = "";
	/// <summary>호스트 주소</summary>
	public string Host { get; set; } = "";
	/// <summary>포트 번호</summary>
	public int Port { get; set; }
	/// <summary>연결 상태: "Connected", "Offline", "Timeout", "AuthFailed", "Unknown"</summary>
	public string Status { get; set; } = "Unknown";
	/// <summary>응답 시간(밀리초) (실패한 경우 -1)</summary>
	public int ResponseTimeMs { get; set; } = -1;
	/// <summary>연결 실패 시 오류 메시지</summary>
	public string? ErrorMessage { get; set; }
	/// <summary>마지막 확인 타임스탬프 (UTC)</summary>
	public DateTime LastCheckUtc { get; set; }
}

/// <summary>외부 서비스 정보 응답</summary>
public class ExternalServicesInfoResponse
{
	/// <summary>현재 상태가 있는 설정된 외부 서비스 목록</summary>
	public List<ExternalServiceStatus> Services { get; set; } = new();
	/// <summary>데이터 수집 타임스탐프 (UTC)</summary>
	public DateTime CollectedAtUtc { get; set; }
}
