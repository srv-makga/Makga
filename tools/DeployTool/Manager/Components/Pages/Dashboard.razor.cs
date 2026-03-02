using DeployTool.Common.Packets;
using DeployTool.Manager.Services;

namespace DeployTool.Manager.Components.Pages;

/// <summary>Code-behind for Dashboard page. Displays overview of all agents and their external services.</summary>
public partial class Dashboard
{
	private List<AgentClient> _agents = new();
	private Dictionary<string, ExternalServicesInfoResponse?> _servicesCache = new();
	private System.Timers.Timer? _timer;

	/// <summary>Initializes the page and starts UI refresh timer.</summary>
	protected override void OnInitialized()
	{
		_agents = Registry.All.ToList();

		_timer = new System.Timers.Timer(TimeSpan.FromSeconds(5));
		_timer.Elapsed += async (_, _) => await RefreshServicesAsync();
		_timer.Start();
		
		RefreshServicesAsync().ConfigureAwait(false);
	}

	/// <summary>Refreshes external services status for all connected agents.</summary>
	private async Task RefreshServicesAsync()
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
		}
		await InvokeAsync(StateHasChanged);
	}

	/// <summary>Gets cached external services for an agent.</summary>
	private ExternalServicesInfoResponse? GetServicesCached(string agentName) =>
		_servicesCache.TryGetValue(agentName, out var services) ? services : null;

	/// <summary>Disposes resources.</summary>
	public void Dispose() => _timer?.Dispose();
}
