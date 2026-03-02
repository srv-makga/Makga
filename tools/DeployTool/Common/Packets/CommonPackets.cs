namespace DeployTool.Common.Packets;

/// <summary>매니저에서 에이전트로의 연결 토큰을 사용한 인증 요청</summary>
public class PingRequest
{
	/// <summary>연결 확인을 위한 인증 토큰</summary>
	public string Token { get; set; } = "";
}

/// <summary>가동 시간 정보가 있는 에이전트의 인증 응답</summary>
public class PongResponse
{
	/// <summary>시작 이후 에이전트 가동 시간(초)</summary>
	public ulong UptimeSec { get; set; }
}

/// <summary>버전, OS, 호스트명 및 가동 시간을 포함한 기본 에이전트 정보</summary>
public class AgentInfoResponse
{
	/// <summary>에이전트 버전 문자열</summary>
	public string Version   { get; set; } = "";
	/// <summary>운영 체제 설명</summary>
	public string Os        { get; set; } = "";
	/// <summary>머신 호스트명</summary>
	public string Hostname  { get; set; } = "";
	/// <summary>현재 작업 디렉터리</summary>
	public string WorkDir   { get; set; } = "";
	/// <summary>에이전트 가동 시간(초)</summary>
	public ulong  UptimeSec { get; set; }
}

/// <summary>단순 작업의 일반 성공/실패 응답</summary>
public class ResultResponse
{
	/// <summary>작업이 성공했으면 true, 아니면 false</summary>
	public bool   Success { get; set; }
	/// <summary>사람이 읽을 수 있는 결과 메시지</summary>
	public string Message { get; set; } = "";
	/// <summary>숫자 결과 코드 (HTTP 유사: 200=성공, 400=클라이언트 오류, 500=서버 오류)</summary>
	public int    Code    { get; set; }
}
