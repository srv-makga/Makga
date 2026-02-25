namespace DeployTool.Protocol.Models;

public class ServerEntry
{
	public string       Name        { get; set; } = "";
	public string       Host        { get; set; } = "";
	public int          Port        { get; set; } = 7700;
	public string       Token       { get; set; } = "";
	public string       Description { get; set; } = "";
	public string       Group       { get; set; } = "";
	public List<string> Tags        { get; set; } = new();
}
