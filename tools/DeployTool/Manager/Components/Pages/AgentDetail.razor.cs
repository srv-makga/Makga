using DeployTool.Common;
using DeployTool.Common.Net;
using DeployTool.Common.Packets;
using DeployTool.Manager.Services;
using Microsoft.AspNetCore.Components;

namespace DeployTool.Manager.Components.Pages;

/// <summary>Code-behind for Agent detail page. Displays Agent information, resources, and external services.</summary>
public partial class AgentDetail
{
	private AgentClient? _client;
	private bool _notFound;
	private AgentInfoResponse? _info;
	private ResourceInfoResponse? _resource;
	private NetworkInfoResponse? _network;
	private ExternalServicesInfoResponse? _services;
	private string _filePath = "";
	private List<FileEntry>? _files;

	/// <summary>Agent name parameter from URL</summary>
	[Parameter] public string Name { get; set; } = "";

	/// <summary>Loads Agent information on initialization.</summary>
	protected override async Task OnInitializedAsync()
	{
		_client = Registry.Get(Name);
		if (null == _client)
		{
			_notFound = true;
			return;
		}

		var resp = await _client.SendEmptyAsync(PacketId.GetInfo);
		if (resp.HasValue)
			_info = PacketSerializer.Deserialize<AgentInfoResponse>(resp.Value.payload);
	}

	/// <summary>Loads resource, network, and external services information.</summary>
	private async Task LoadResourceAsync()
	{
		if (null == _client) return;

		var resp = await _client.SendEmptyAsync(PacketId.GetResource);
		if (resp.HasValue)
			_resource = PacketSerializer.Deserialize<ResourceInfoResponse>(resp.Value.payload);

		var netResp = await _client.SendEmptyAsync(PacketId.GetNetwork);
		if (netResp.HasValue)
			_network = PacketSerializer.Deserialize<NetworkInfoResponse>(netResp.Value.payload);

		var svcResp = await _client.GetExternalServicesAsync();
		_services = svcResp;
	}

	/// <summary>Loads directory listing from specified path.</summary>
	private async Task LoadFilesAsync()
	{
		if (null == _client) return;

		var resp = await _client.SendAsync(PacketId.ListFiles, new ListFilesRequest { Path = _filePath });
		if (resp.HasValue)
		{
			var result = PacketSerializer.Deserialize<FileListResponse>(resp.Value.payload);
			_files = result?.Files;
		}
	}

	/// <summary>Formats bytes to human-readable string.</summary>
	private static string FormatBytes(long bytes)
	{
		if (bytes >= 1_073_741_824) return $"{bytes / 1_073_741_824.0:0.0} GB";
		if (bytes >= 1_048_576)     return $"{bytes / 1_048_576.0:0.0} MB";
		if (bytes >= 1_024)         return $"{bytes / 1_024.0:0.0} KB";
		return $"{bytes} B";
	}
}
