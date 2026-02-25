using System.Text.Json;
using System.Text.Json.Nodes;
using DeployTool.Protocol.Packets;
using Microsoft.Extensions.Configuration;

namespace DeployTool.Agent.Services;

public class ConfigService
{
	private readonly string _configPath;
	private readonly IConfiguration _configuration;

	public ConfigService(IConfiguration configuration)
	{
		_configuration = configuration;
		_configPath    = Path.Combine(AppContext.BaseDirectory, "appsettings.json");
	}

	public async Task<ConfigDataResponse> GetConfigAsync()
	{
		var json = await File.ReadAllTextAsync(_configPath);
		return new ConfigDataResponse { Json = json };
	}

	// @brief key:value 쌍을 JSON 경로(":"로 구분된) 기준으로 appsettings.json에 부분 업데이트
	public async Task<ResultResponse> SetConfigAsync(SetConfigRequest req)
	{
		var json = await File.ReadAllTextAsync(_configPath);
		var root = JsonNode.Parse(json) as JsonObject
			?? throw new InvalidOperationException("Invalid appsettings.json");

		foreach (var (key, value) in req.Changes)
		{
			var parts   = key.Split(':');
			var current = root as JsonObject;

			for (var i = 0; i < parts.Length - 1; i++)
			{
				if (current![parts[i]] is not JsonObject child)
				{
					child = new JsonObject();
					current[parts[i]] = child;
				}
				current = child;
			}
			current![parts[^1]] = JsonValue.Create(value);
		}

		var updated = root.ToJsonString(new JsonSerializerOptions { WriteIndented = true });
		await File.WriteAllTextAsync(_configPath, updated);
		return new ResultResponse { Success = true };
	}

	public Task<ResultResponse> ReloadConfigAsync()
	{
		if (_configuration is IConfigurationRoot root)
			root.Reload();
		return Task.FromResult(new ResultResponse { Success = true, Message = "Config reloaded" });
	}
}
