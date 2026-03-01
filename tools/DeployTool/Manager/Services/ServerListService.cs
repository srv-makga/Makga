using System.Text.Json;
using DeployTool.Common.Models;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace DeployTool.Manager.Services;

public class ServerListService : IHostedService, IDisposable
{
	private readonly ManagerConfig    _cfg;
	private readonly AgentRegistry    _registry;
	private readonly ILogger<ServerListService> _log;
	private FileSystemWatcher? _watcher;

	public ServerListService(IOptions<ManagerConfig> cfg, AgentRegistry registry, ILogger<ServerListService> log)
	{
		_cfg      = cfg.Value;
		_registry = registry;
		_log      = log;
	}

	public Task StartAsync(CancellationToken ct)
	{
		LoadServerList();

		// serverlist.json 변경 감지 → 자동 재로드
		var dir  = Path.GetDirectoryName(Path.GetFullPath(_cfg.ServerListPath)) ?? ".";
		var file = Path.GetFileName(_cfg.ServerListPath);
		_watcher = new FileSystemWatcher(dir, file)
		{
			NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.Size,
			EnableRaisingEvents = true
		};
		_watcher.Changed += (_, _) => LoadServerList();

		return Task.CompletedTask;
	}

	public Task StopAsync(CancellationToken ct)
	{
		_watcher?.Dispose();
		return Task.CompletedTask;
	}

	private void LoadServerList()
	{
		try
		{
			var path = Path.GetFullPath(_cfg.ServerListPath);
			if (!File.Exists(path))
			{
				_log.LogWarning("serverlist.json not found at {Path}", path);
				return;
			}
			var json = File.ReadAllText(path);
			var servers = JsonSerializer.Deserialize<ServerList>(json,
				new JsonSerializerOptions { PropertyNameCaseInsensitive = true });

			if (null != servers)
				_ = _registry.UpdateServers(servers.Servers);

			_log.LogInformation("Loaded {Count} servers from serverlist.json", servers?.Servers.Count ?? 0);
		}
		catch (Exception ex)
		{
			_log.LogError(ex, "Failed to load serverlist.json");
		}
	}

	public void Dispose() => _watcher?.Dispose();
}
