using System.Text.Json;
using System.Text.Json.Nodes;
using Microsoft.Extensions.Options;

namespace DeployTool.Manager.Services;

// @brief Manager 설정(RetryIntervalSec, HeartbeatIntervalSec)을 appsettings.json에 저장
public class ManagerSettingsService
{
	private readonly string                         _path;
	private readonly IOptionsMonitor<ManagerConfig> _options;

	public ManagerSettingsService(IWebHostEnvironment env, IOptionsMonitor<ManagerConfig> options)
	{
		_path    = Path.Combine(env.ContentRootPath, "appsettings.json");
		_options = options;
	}

	public ManagerConfig Current => _options.CurrentValue;

	public async Task SaveAsync(int retryIntervalSec, int heartbeatIntervalSec)
	{
		var text = await File.ReadAllTextAsync(_path);
		var node = JsonNode.Parse(text)
			?? throw new InvalidOperationException("appsettings.json parse failed");

		node["DeployManager"]!["RetryIntervalSec"]     = retryIntervalSec;
		node["DeployManager"]!["HeartbeatIntervalSec"] = heartbeatIntervalSec;

		await File.WriteAllTextAsync(_path,
			node.ToJsonString(new JsonSerializerOptions { WriteIndented = true }));
	}
}
