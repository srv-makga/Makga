using System.Collections.Concurrent;
using DeployTool.Common.Models;
using Microsoft.Extensions.Options;

namespace DeployTool.Manager.Services;

/// <summary>
/// 모든 설정된 서버에 대한 AgentClient 인스턴스 모음을 관리합니다.
/// 클라이언트 수명 주기, 연결 업데이트를 처리하고 스레드로부터 안전한 액세스를 제공합니다.
/// </summary>
public class AgentRegistry : IAsyncDisposable
{
	private readonly IOptionsMonitor<ManagerConfig>            _options;
	private readonly ConcurrentDictionary<string, AgentClient> _clients = new();

	/// <summary>
	/// AgentRegistry 클래스의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="options">매니저 설정 옵션</param>
	public AgentRegistry(IOptionsMonitor<ManagerConfig> options) => _options = options;

	/// <summary>
	/// 등록된 모든 에이전트 클라이언트의 읽기 전용 모음을 가져옵니다.
	/// </summary>
	public IReadOnlyCollection<AgentClient> All => _clients.Values.ToList();

	/// <summary>
	/// 이름별로 특정 에이전트 클라이언트를 가져옵니다.
	/// </summary>
	/// <param name="name">서버 이름</param>
	/// <returns>찾을 수 없으면 AgentClient 또는 null</returns>
	public AgentClient? Get(string name) =>
		_clients.TryGetValue(name, out var c) ? c : null;

	/// <summary>
	/// 새 서버 목록으로 레지스트리를 업데이트합니다.
	/// 더 이상 목록에 없는 서버의 연결을 끊고 새 서버에 대한 클라이언트를 만듭니다.
	/// 기존 연결은 유지됩니다.
	/// </summary>
	/// <param name="servers">새 서버 설정 목록</param>
	/// <returns>완료 작업</returns>
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

	/// <summary>
	/// 모든 에이전트 클라이언트를 처리하고 레지스트리를 지웁니다.
	/// </summary>
	/// <returns>완료 작업</returns>
	public async ValueTask DisposeAsync()
	{
		foreach (var client in _clients.Values)
			await client.DisposeAsync();
		_clients.Clear();
	}
}
