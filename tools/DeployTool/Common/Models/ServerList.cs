namespace DeployTool.Common.Models;

/// <summary>Wrapper for a list of server configurations loaded from serverlist.json.</summary>
public class ServerList
{
	/// <summary>List of server entries</summary>
	public List<ServerEntry> Servers { get; set; } = new();
}
