using DeployTool.Manager.Services;
using DeployTool.Common.Packets;

namespace DeployTool.Manager.Components.Pages;

/// <summary>Code-behind for Agents list page. Displays all agents with filtering and grouping.</summary>
public partial class Agents
{
	private List<AgentClient> _agents = new();
	private List<string> _groups = new();
	private List<string> _allTags = new();
	private string? _selectedGroup;
	private string _searchText = "";
	private HashSet<string> _selectedTags = new();
	private HashSet<string> _expandedAgents = new();
	// (AgentName, Uid) — Status는 저장하지 않고 캐시에서 실시간으로 조회
	private HashSet<(string AgentName, string Uid)> _selectedProcesses = new();
	private Dictionary<string, MonitoredProcessesResponse?> _processCache = new();
	private bool _controllingProcess = false;
	private System.Timers.Timer? _timer;

	/// <summary>Filtered agent list based on selected criteria.</summary>
	private IEnumerable<AgentClient> Filtered =>
		_agents
			.Where(a => _selectedGroup == null || a.Info.Group == _selectedGroup)
			.Where(a => string.IsNullOrEmpty(_searchText) ||
			            a.Info.Name.Contains(_searchText, StringComparison.OrdinalIgnoreCase) ||
			            a.Info.Description.Contains(_searchText, StringComparison.OrdinalIgnoreCase) ||
			            a.Info.Host.Contains(_searchText, StringComparison.OrdinalIgnoreCase))
			.Where(a => _selectedTags.Count == 0 || _selectedTags.All(t => a.Info.Tags.Contains(t)));

	/// <summary>Initializes the page and starts UI refresh timer.</summary>
	protected override void OnInitialized()
	{
		_agents  = Registry.All.ToList();
		_groups  = _agents.Select(a => a.Info.Group).Where(g => !string.IsNullOrEmpty(g)).Distinct().Order().ToList();
		_allTags = _agents.SelectMany(a => a.Info.Tags).Distinct().Order().ToList();

		_timer = new System.Timers.Timer(TimeSpan.FromSeconds(3));
		_timer.Elapsed += async (_, _) =>
		{
			await RefreshProcessesAsync();
			await InvokeAsync(StateHasChanged);
		};
		_timer.Start();
	}

	/// <summary>Selects a group for filtering.</summary>
	private void SelectGroup(string? group) => _selectedGroup = group;

	/// <summary>Toggles a tag selection.</summary>
	private void ToggleTag(string tag)
	{
		if (!_selectedTags.Remove(tag))
			_selectedTags.Add(tag);
	}

	/// <summary>Clears all selected tags.</summary>
	private void ClearTags() => _selectedTags.Clear();

	/// <summary>Toggles agent expansion to show/hide processes.</summary>
	private void ToggleAgentExpand(string agentName)
	{
		if (_expandedAgents.Contains(agentName))
			_expandedAgents.Remove(agentName);
		else
			_expandedAgents.Add(agentName);
	}

	/// <summary>Toggles process selection for start/stop operations.</summary>
	private void ToggleProcessSelection(string agentName, MonitoredProcessStatus proc, bool selected)
	{
		var key = (agentName, proc.Uid);
		if (selected)
			_selectedProcesses.Add(key);
		else
			_selectedProcesses.Remove(key);
	}

	/// <summary>캐시에서 해당 에이전트 + uid의 현재 Status를 반환합니다.</summary>
	private string GetCachedStatus(string agentName, string uid)
	{
		if (_processCache.TryGetValue(agentName, out var resp) && resp != null)
		{
			var p = resp.Processes.FirstOrDefault(p => p.Uid == uid);
			if (p != null) return p.Status;
		}
		return "Unknown";
	}

	/// <summary>Gets cached processes for an agent, returns null if not loaded.</summary>
	private MonitoredProcessesResponse? GetProcessesCached(string agentName)
	{
		return _processCache.ContainsKey(agentName) ? _processCache[agentName] : null;
	}

	/// <summary>Checks if start button should be disabled.</summary>
	private bool GetStartDisabled(string agentName)
	{
		if (_controllingProcess) return true;
		return !_selectedProcesses
			.Where(p => p.AgentName == agentName)
			.Any(p => GetCachedStatus(agentName, p.Uid) != "Running");
	}

	/// <summary>Checks if stop button should be disabled.</summary>
	private bool GetStopDisabled(string agentName)
	{
		if (_controllingProcess) return true;
		return !_selectedProcesses
			.Where(p => p.AgentName == agentName)
			.Any(p => GetCachedStatus(agentName, p.Uid) == "Running");
	}

	/// <summary>Refreshes process list for all connected agents.</summary>
	private async Task RefreshProcessesAsync()
	{
		foreach (var agent in _agents.Where(a => a.IsConnected && _expandedAgents.Contains(a.Info.Name)))
		{
			try
			{
				var resp = await agent.GetMonitoredProcessesAsync();
				if (resp != null)
					_processCache[agent.Info.Name] = resp;
			}
			catch { }
		}
	}

	/// <summary>Starts selected processes for an agent.</summary>
	private async Task StartSelectedProcessesAsync(string agentName, List<MonitoredProcessStatus> allProcesses)
	{
		_controllingProcess = true;
		try
		{
			var selected = _selectedProcesses
				.Where(p => p.AgentName == agentName && GetCachedStatus(agentName, p.Uid) != "Running")
				.Select(p => p.Uid)
				.ToList();
			var client = Registry.Get(agentName);

			if (client == null || !client.IsConnected) return;

			foreach (var uid in selected)
			{
				await client.StartMonitoredProcessAsync(uid);
				await Task.Delay(500);
			}

			await RefreshProcessesAsync();
			await InvokeAsync(StateHasChanged);
		}
		finally
		{
			_controllingProcess = false;
		}
	}

	/// <summary>Stops selected processes for an agent.</summary>
	private async Task StopSelectedProcessesAsync(string agentName, List<MonitoredProcessStatus> allProcesses)
	{
		_controllingProcess = true;
		try
		{
			var selected = _selectedProcesses
				.Where(p => p.AgentName == agentName && GetCachedStatus(agentName, p.Uid) == "Running")
				.Select(p => p.Uid)
				.ToList();
			var client = Registry.Get(agentName);

			if (client == null || !client.IsConnected) return;

			foreach (var uid in selected)
			{
				await client.StopMonitoredProcessAsync(uid);
				await Task.Delay(500);
			}

			await RefreshProcessesAsync();
			await InvokeAsync(StateHasChanged);
		}
		finally
		{
			_controllingProcess = false;
		}
	}

	/// <summary>Disposes resources.</summary>
	public void Dispose() => _timer?.Dispose();
}
