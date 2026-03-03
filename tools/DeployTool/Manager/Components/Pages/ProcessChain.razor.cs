namespace DeployTool.Manager.Components.Pages;

using DeployTool.Common.Packets;
using DeployTool.Manager.Services;
using Microsoft.AspNetCore.Components;

public partial class ProcessChain : ComponentBase, IDisposable
{
	private List<AgentClient>? _agents;
	private Dictionary<string, List<MonitoredProcessStatus>> _agentProcesses = new();
	private HashSet<string> _expandedAgents = new();
	private LinkedDictionary<string, (string AgentName, string ProcessDisplayName, string ProcessUid)> _selectedProcesses = new();

	private bool _executing = false;
	private List<ExecutionLogEntry> _executionLog = new();

	protected override async Task OnInitializedAsync()
	{
		_agents = Registry.All.ToList();

		// 각 Agent의 프로세스 로드
		foreach (var agent in _agents)
		{
			await LoadAgentProcessesAsync(agent);
		}

		foreach (var a in Registry.All)
		{
			a.OnStateChanged += StateHasChanged;
		}
	}

	private async Task LoadAgentProcessesAsync(AgentClient agent)
	{
		if (!agent.IsConnected)
			return;

		try
		{
			var resp = await agent.GetMonitoredProcessesAsync();
			if (resp != null)
			{
				_agentProcesses[agent.Info.Name] = resp.Processes;
			}
		}
		catch { }
	}

	private void ToggleAgentExpand(string agentName)
	{
		if (_expandedAgents.Contains(agentName))
			_expandedAgents.Remove(agentName);
		else
			_expandedAgents.Add(agentName);
	}

	private void ToggleProcessSelection(string processId, string agentName, MonitoredProcessStatus proc, bool selected)
	{
		if (selected)
		{
			_selectedProcesses[processId] = (agentName, proc.DisplayName, proc.Uid);
		}
		else
		{
			_selectedProcesses.Remove(processId);
		}
	}

	private void RemoveFromSelection(string processId)
	{
		_selectedProcesses.Remove(processId);
	}

	private async Task ClearSelectionAsync()
	{
		_selectedProcesses.Clear();
		await Task.CompletedTask;
	}

	private async Task ExecuteChainAsync()
	{
		if (_selectedProcesses.Count == 0)
			return;

		_executing = true;
		_executionLog.Clear();

		try
		{
			foreach (var (processId, (agentName, displayName, uid)) in _selectedProcesses.OrderedItems)
			{
				var agent = Registry.Get(agentName);
				if (agent == null)
				{
					AddLog($"Agent '{agentName}' 를 찾을 수 없음", false);
					continue;
				}

				if (!agent.IsConnected)
				{
					AddLog($"Agent '{agentName}' 연결 끊김", false);
					continue;
				}

				// 프로세스 시작 (이미 실행중이면 skip)
				var proc = _agentProcesses[agentName].FirstOrDefault(p => p.Uid == uid);
				if (proc?.Status == "Running")
				{
					AddLog($"[{agentName}] {displayName} 이미 실행 중 (Skip)", true);
					await Task.Delay(500);
					continue;
				}

				AddLog($"[{agentName}] {displayName} 시작 요청...", true);
				var result = await agent.StartMonitoredProcessAsync(uid);

				if (result?.Success == true)
				{
					AddLog($"[{agentName}] {displayName} 시작됨", true);

					// 프로세스 시작 확인 (최대 10초 대기)
					bool confirmed = false;
					for (int i = 0; i < 20; i++)
					{
						await Task.Delay(500);
						var procResp = await agent.GetMonitoredProcessesAsync();
						if (procResp?.Processes.Any(p => p.Uid == uid && p.Status == "Running") == true)
						{
							AddLog($"[{agentName}] {displayName} 확인됨 ✓", true);
							_agentProcesses[agentName] = procResp.Processes;
							confirmed = true;
							break;
						}
					}

					if (!confirmed)
					{
						AddLog($"[{agentName}] {displayName} 확인 실패 (Timeout)", false);
					}
				}
				else
				{
					AddLog($"[{agentName}] {displayName} 시작 실패: {result?.Message}", false);
				}

				// 다음 프로세스 실행 전 딜레이
				await Task.Delay(1000);
			}

			AddLog("프로세스 체인 실행 완료", true);
		}
		finally
		{
			_executing = false;
		}
	}

	private void AddLog(string message, bool success)
	{
		_executionLog.Add(new ExecutionLogEntry
		{
			Message = message,
			IsSuccess = success,
			Timestamp = DateTime.Now
		});
	}

	private int GetSelectedProcessCount() => _selectedProcesses.Count;

	public void Dispose()
	{
		foreach (var a in Registry.All)
		{
			a.OnStateChanged -= StateHasChanged;
		}
	}
}

public class ExecutionLogEntry
{
	public string Message { get; set; } = "";
	public bool IsSuccess { get; set; }
	public DateTime Timestamp { get; set; }
}

// LinkedDictionary는 삽입 순서를 유지하는 Dictionary
public class LinkedDictionary<K, V> : Dictionary<K, V> where K : notnull
{
	private List<K> _order = new();

	public new void Add(K key, V value)
	{
		if (!ContainsKey(key))
			_order.Add(key);
		base[key] = value;
	}

	public new V this[K key]
	{
		get => base[key];
		set
		{
			if (!ContainsKey(key))
				_order.Add(key);
			base[key] = value;
		}
	}

	public new bool Remove(K key)
	{
		_order.Remove(key);
		return base.Remove(key);
	}

	public new void Clear()
	{
		_order.Clear();
		base.Clear();
	}

	public int Count => base.Count;

	public IEnumerable<(K Key, V Value)> OrderedItems
	{
		get => _order.Where(k => ContainsKey(k)).Select(k => (k, base[k]));
	}

	public IEnumerator<(K Key, V Value)> GetEnumeratorOrdered()
	{
		foreach (var k in _order)
		{
			if (ContainsKey(k))
				yield return (k, base[k]);
		}
	}
}
