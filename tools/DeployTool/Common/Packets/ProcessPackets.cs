namespace DeployTool.Common.Packets;

// ── Manager → Agent ───────────────────────────────────────────
public class ExecuteRequest
{
	public string       Program { get; set; } = "";
	public List<string> Args    { get; set; } = new();
	public string       WorkDir { get; set; } = "";
	public bool         Detach  { get; set; } = false;
}

public class KillRequest        { public int Pid { get; set; } }
public class ServiceNameRequest { public string Name { get; set; } = ""; } // ServiceStart / Stop / Status

// ── Agent → Manager ───────────────────────────────────────────
public class ProcessEntry
{
	public int    Pid    { get; set; }
	public string Name   { get; set; } = "";
	public float  CpuPct { get; set; }
	public float  MemMb  { get; set; }
}

public class ProcessListResponse
{
	public List<ProcessEntry> Processes { get; set; } = new();
}

public class ServiceEntry
{
	public string Name        { get; set; } = "";
	public string DisplayName { get; set; } = "";
	public string Status      { get; set; } = ""; // Running / Stopped / etc.
}

public class ServiceStatusResponse
{
	public string Name   { get; set; } = "";
	public string Status { get; set; } = "";
}

public class ServiceListResponse
{
	public List<ServiceEntry> Services { get; set; } = new();
}
