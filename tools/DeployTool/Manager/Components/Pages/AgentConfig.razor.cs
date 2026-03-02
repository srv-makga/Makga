using DeployTool.Common;
using DeployTool.Common.Net;
using DeployTool.Common.Packets;
using DeployTool.Manager.Services;
using Microsoft.AspNetCore.Components;

namespace DeployTool.Manager.Components.Pages;

/// <summary>에이전트 설정 페이지의 코드 비하인드. 에이전트 설정을 보고 편집할 수 있습니다.</summary>
public partial class AgentConfig
{
	private AgentClient? _client;
	private bool _notFound;
	private string _configJson = "";
	private bool _saving;
	private string _message = "";

	/// <summary>URL의 에이전트 이름 매개변수</summary>
	[Parameter] public string Name { get; set; } = "";

	/// <summary>초기화 시 에이전트 설정을 로드합니다.</summary>
	protected override async Task OnInitializedAsync()
	{
		_client = Registry.Get(Name);
		if (null == _client)
		{
			_notFound = true;
			return;
		}

		var resp = await _client.SendEmptyAsync(PacketId.GetConfig);
		if (resp.HasValue)
		{
			var data = PacketSerializer.Deserialize<ConfigDataResponse>(resp.Value.payload);
			_configJson = data?.Json ?? "{}";
		}
	}

	/// <summary>설정 변경 사항을 에이전트에 저장합니다.</summary>
	private async Task SaveAsync()
	{
		if (null == _client) return;
		_saving  = true;
		_message = "";

		var changes = new Dictionary<string, string> { { "_raw", _configJson } };
		await _client.SendAsync(PacketId.SetConfig, new SetConfigRequest { Changes = changes });

		_message = L["설정_저장완료"];
		_saving  = false;
	}
}
