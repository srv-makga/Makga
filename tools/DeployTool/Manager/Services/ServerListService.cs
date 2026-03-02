using System.Text.Json;
using DeployTool.Common.Models;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace DeployTool.Manager.Services;

/// <summary>
/// Watches and loads server list from serverlist.json file.
/// Automatically detects changes and updates the agent registry.
/// </summary>
public class ServerListService : IHostedService, IDisposable
{
	private readonly ManagerConfig    _cfg;
	private readonly AgentRegistry    _registry;
	private readonly ILogger<ServerListService> _log;
	private FileSystemWatcher? _watcher;
	private DateTime _lastLoadTime = DateTime.MinValue;

	/// <summary>
	/// Initializes a new instance of the ServerListService class.
	/// </summary>
	/// <param name="cfg">Manager configuration options</param>
	/// <param name="registry">Agent registry for updating connections</param>
	/// <param name="log">Logger instance</param>
	public ServerListService(IOptions<ManagerConfig> cfg, AgentRegistry registry, ILogger<ServerListService> log)
	{
		_cfg      = cfg.Value;
		_registry = registry;
		_log      = log;
	}

	/// <summary>
	/// Starts the hosted service, loading initial server list and setting up file watcher.
	/// </summary>
	/// <param name="ct">Cancellation token</param>
	/// <returns>Completion task</returns>
	public Task StartAsync(CancellationToken ct)
	{
		LoadServerList();

		var dir  = Path.GetDirectoryName(Path.GetFullPath(_cfg.ServerListPath)) ?? ".";
		var file = Path.GetFileName(_cfg.ServerListPath);
		_watcher = new FileSystemWatcher(dir, file)
		{
			NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.Size,
			EnableRaisingEvents = true
		};
		_watcher.Changed += (_, _) =>
		{
			var now = DateTime.UtcNow;
			if ((now - _lastLoadTime).TotalMilliseconds >= 100)
			{
				_lastLoadTime = now;
				LoadServerList();
			}
		};

		return Task.CompletedTask;
	}

	/// <summary>
	/// Stops the hosted service and disposes resources.
	/// </summary>
	/// <param name="ct">Cancellation token</param>
	/// <returns>Completion task</returns>
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
			// .NET API: JsonSerializerOptions(PropertyNameCaseInsensitive=true)で大文字小文字を区別しない JSON デシリアライゼーション
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

	/// <summary>Releases file watcher resources.</summary>
	public void Dispose() => _watcher?.Dispose();
}
