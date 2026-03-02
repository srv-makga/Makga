using DeployTool.Manager.Services;

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
		_timer.Elapsed += async (_, _) => await InvokeAsync(StateHasChanged);
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

	/// <summary>Disposes resources.</summary>
	public void Dispose() => _timer?.Dispose();
}
