namespace DeployTool.Protocol.Packets;

// ── Manager → Agent: 연결 ──────────────────────────────────────
public class PingRequest
{
	public string Token { get; set; } = "";
}

// ── Agent → Manager: 연결 응답 ────────────────────────────────
public class PongResponse
{
	public ulong UptimeSec { get; set; }
}

public class AgentInfoResponse
{
	public string Version   { get; set; } = "";
	public string Os        { get; set; } = "";
	public string Hostname  { get; set; } = "";
	public string WorkDir   { get; set; } = "";
	public ulong  UptimeSec { get; set; }
}

// @brief 범용 성공/실패 응답
public class ResultResponse
{
	public bool   Success { get; set; }
	public string Message { get; set; } = "";
	public int    Code    { get; set; }
}
