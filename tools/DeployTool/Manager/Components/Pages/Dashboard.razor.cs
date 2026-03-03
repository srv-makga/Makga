using DeployTool.Common.Packets;
using DeployTool.Manager.Services;

namespace DeployTool.Manager.Components.Pages;

/// <summary>Code-behind for Dashboard page. Displays overview of all agents, external services, and monitored processes.</summary>
public partial class Dashboard
{
	private List<AgentClient> _agents = new();
	private Dictionary<string, ExternalServicesInfoResponse?> _servicesCache = new();
	private Dictionary<string, MonitoredProcessesResponse?> _processesCache = new();
	private System.Timers.Timer? _timer;

	/// <summary>Initializes the page and starts UI refresh timer.</summary>
	protected override void OnInitialized()
	{
		_agents = Registry.All.ToList();

		_timer = new System.Timers.Timer(TimeSpan.FromSeconds(5));
		_timer.Elapsed += async (_, _) => await RefreshDataAsync();
		_timer.Start();
		
		RefreshDataAsync().ConfigureAwait(false);
	}

	/// <summary>Refreshes external services and monitored processes status for all connected agents.</summary>
	private async Task RefreshDataAsync()
	{
		foreach (var agent in _agents.Where(a => a.IsConnected))
		{
			try
			{
				var services = await agent.GetExternalServicesAsync();
				_servicesCache[agent.Info.Name] = services;
			}
			catch
			{
				_servicesCache[agent.Info.Name] = null;
			}

			try
			{
				var processes = await agent.GetMonitoredProcessesAsync();
				_processesCache[agent.Info.Name] = processes;
			}
			catch
			{
				_processesCache[agent.Info.Name] = null;
			}
		}
		await InvokeAsync(StateHasChanged);
	}

	/// <summary>Gets cached external services for an agent.</summary>
	private ExternalServicesInfoResponse? GetServicesCached(string agentName) =>
		_servicesCache.TryGetValue(agentName, out var services) ? services : null;

	/// <summary>Gets cached monitored processes for an agent.</summary>
	private MonitoredProcessesResponse? GetProcessesCached(string agentName) =>
		_processesCache.TryGetValue(agentName, out var processes) ? processes : null;

	/// <summary>Disposes resources.</summary>
	public void Dispose() => _timer?.Dispose();
}
