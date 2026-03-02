namespace DeployTool.Common.Models;

/// <summary>Configuration entry for a single server/agent that Manager can connect to.</summary>
public class ServerEntry
{
	/// <summary>Unique server name identifier</summary>
	public string       Name        { get; set; } = "";
	/// <summary>Server hostname or IP address</summary>
	public string       Host        { get; set; } = "";
	/// <summary>TCP port for agent connection (default: 7700)</summary>
	public int          Port        { get; set; } = 7700;
	/// <summary>Authentication token for secure connection</summary>
	public string       Token       { get; set; } = "";
	/// <summary>Human-readable server description</summary>
	public string       Description { get; set; } = "";
	/// <summary>Server group for organization and filtering</summary>
	public string       Group       { get; set; } = "";
	/// <summary>Tags for categorization and filtering</summary>
	public List<string> Tags        { get; set; } = new();
}
