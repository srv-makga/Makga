using System.Collections.Concurrent;
using DeployTool.Protocol.Models;

namespace DeployTool.Manager.Services;

// @brief 등록된 모든 AgentClient를 관리하는 Singleton
public class AgentRegistry
{
	private readonly ConcurrentDictionary<string, AgentClient> _clients = new();

	public IReadOnlyCollection<AgentClient> All => _clients.Values.ToList();

	public AgentClient? Get(string name) =>
		_clients.TryGetValue(name, out var c) ? c : null;

	// @brief serverlist.json 재로드 시 호출 — 추가/제거 반영
	public void UpdateServers(List<ServerEntry> servers)
	{
		var incoming = servers.ToDictionary(s => s.Name);

		// 제거된 서버 연결 해제
		foreach (var name in _clients.Keys.Except(incoming.Keys).ToList())
		{
			if (_clients.TryRemove(name, out var old))
				old.Dispose();
		}

		// 신규 서버 등록
		foreach (var entry in servers)
		{
			_clients.TryAdd(entry.Name, new AgentClient(entry));
		}
	}
}
