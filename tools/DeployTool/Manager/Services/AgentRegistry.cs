using System.Collections.Concurrent;
using DeployTool.Common.Models;
using Microsoft.Extensions.Options;

namespace DeployTool.Manager.Services;

// @brief 등록된 모든 AgentClient를 관리하는 Singleton
public class AgentRegistry : IAsyncDisposable
{
	private readonly IOptionsMonitor<ManagerConfig>            _options;
	private readonly ConcurrentDictionary<string, AgentClient> _clients = new();

	public AgentRegistry(IOptionsMonitor<ManagerConfig> options) => _options = options;

	public IReadOnlyCollection<AgentClient> All => _clients.Values.ToList();

	public AgentClient? Get(string name) =>
		_clients.TryGetValue(name, out var c) ? c : null;

	// @brief serverlist.json 재로드 시 호출 — 추가/제거 반영
	public async Task UpdateServers(List<ServerEntry> servers)
	{
		var incoming = servers.ToDictionary(s => s.Name);

		// 제거된 서버 연결 해제
		foreach (var name in _clients.Keys.Except(incoming.Keys).ToList())
		{
			if (_clients.TryRemove(name, out var old))
				await old.DisposeAsync();
		}

		// 신규 서버 등록 (기존 서버는 유지)
		foreach (var entry in servers)
		{
			_clients.TryAdd(entry.Name, new AgentClient(entry, _options));
		}
	}

	public async ValueTask DisposeAsync()
	{
		foreach (var client in _clients.Values)
			await client.DisposeAsync();
		_clients.Clear();
	}
}
