namespace DeployTool.Common.Packets;

// ── Manager → Agent ───────────────────────────────────────────
public class CheckPortRequest { public int Port { get; set; } }

// ── Agent → Manager ───────────────────────────────────────────
public class ResourceInfoResponse
{
	public float  CpuPct      { get; set; }
	public long   MemUsedMb   { get; set; }
	public long   MemTotalMb  { get; set; }
	public double DiskUsedGb  { get; set; }
	public double DiskTotalGb { get; set; }
	public string Os          { get; set; } = "";
}

public class NetworkInterfaceEntry
{
	public string Name      { get; set; } = "";
	public long   BytesSent { get; set; }
	public long   BytesRecv { get; set; }
}

public class NetworkInfoResponse
{
	public List<NetworkInterfaceEntry> Interfaces { get; set; } = new();
}

public class PortStatusResponse
{
	public int  Port     { get; set; }
	public bool Listening { get; set; }
}
