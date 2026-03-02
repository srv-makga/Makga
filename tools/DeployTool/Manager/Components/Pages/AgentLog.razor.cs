using DeployTool.Common;
using DeployTool.Common.Net;
using DeployTool.Common.Packets;
using DeployTool.Manager.Services;
using Microsoft.AspNetCore.Components;

namespace DeployTool.Manager.Components.Pages;

/// <summary>Code-behind for Agent log viewer page. Displays log file tail.</summary>
public partial class AgentLog
{
	private AgentClient? _client;
	private bool _notFound;
	private string _logFile = "";
	private int _lines = 100;
	private string? _logContent;

	/// <summary>Agent name parameter from URL</summary>
	[Parameter] public string Name { get; set; } = "";

	/// <summary>Initializes the page and gets Agent client reference.</summary>
	protected override void OnInitialized()
	{
		_client = Registry.Get(Name);
		if (null == _client)
			_notFound = true;
	}

	/// <summary>Loads and displays log file content.</summary>
	private async Task LoadLogAsync()
	{
		if (null == _client || string.IsNullOrEmpty(_logFile)) return;

		var resp = await _client.SendAsync(PacketId.GetLog,
			new GetLogRequest { Path = _logFile, Lines = _lines });
		if (resp.HasValue)
		{
			var log = PacketSerializer.Deserialize<LogDataResponse>(resp.Value.payload);
			_logContent = log?.Content;
		}
	}
}
