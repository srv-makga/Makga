using System.Net;
using System.Net.Sockets;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace DeployTool.Agent.Services;

/// <summary>
/// 매니저 연결을 수신 대기하고 IP 주소별로 세션을 관리하는 TCP 서버.
/// 중복 연결을 방지하고 세션 수명 주기를 처리합니다.
/// </summary>
public class AgentServer : BackgroundService
{
	private readonly AgentConfig          _cfg;
	private readonly CommandDispatcher    _dispatcher;
	private readonly ILogger<AgentServer> _log;
	private readonly DateTime             _startTime = DateTime.UtcNow;

	private readonly Dictionary<string, (AgentSession Session, Task RunTask)> _sessions = new();
	private readonly object _sessLock = new();

	/// <summary>
	/// 지정된 설정 및 디스패처를 사용하여 AgentServer 클래스의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="cfg">에이전트 설정 옵션</param>
	/// <param name="dispatcher">패킷 라우팅을 위한 명령 디스패처</param>
	/// <param name="log">로거 인스턴스</param>
	public AgentServer(IOptions<AgentConfig> cfg, CommandDispatcher dispatcher, ILogger<AgentServer> log)
	{
		_cfg        = cfg.Value;
		_dispatcher = dispatcher;
		_log        = log;
	}

	/// <summary>
	/// 백그라운드 서비스를 실행하여 들어오는 TCP 연결을 수신 대기하고 세션을 관리합니다.
	/// </summary>
	/// <param name="ct">서비스를 중지할 취소 토큰</param>
	/// <returns>수신 대기 루프를 나타내는 작업</returns>
	protected override async Task ExecuteAsync(CancellationToken ct)
	{
		var listener = new TcpListener(IPAddress.Any, _cfg.ListenPort);
		listener.Start();
		_log.LogInformation("에이전트 포트 {Port}에서 수신 대기 중", _cfg.ListenPort);

		while (!ct.IsCancellationRequested)
		{
			try
			{
				var client   = await listener.AcceptTcpClientAsync(ct);
				var remoteIp = ((IPEndPoint)client.Client.RemoteEndPoint!).Address.ToString();

				// IP 화이트리스트 검사 — AllowedManagerHosts가 비어있으면 모든 IP 허용
				if (_cfg.AllowedManagerHosts.Count > 0 && !_cfg.AllowedManagerHosts.Contains(remoteIp))
				{
					_log.LogWarning("[AgentServer] 나열되지 않은 IP {IP} 거절", remoteIp);
					client.Dispose();
					continue;
				}

				lock (_sessLock)
				{
					// 살아있는 세션이 있으면 중복 접속 거절
					if (_sessions.TryGetValue(remoteIp, out var existing) && !existing.RunTask.IsCompleted)
					{
						_log.LogWarning("[AgentServer] {IP}로부터의 중복 연결 거절", remoteIp);
						client.Dispose();
						continue;
					}

					// 이전 세션이 죽었거나 없으면 교체
					existing.Session?.Dispose();

					var session = new AgentSession(client, _dispatcher, _cfg, _log, _startTime);
					var runTask = session.RunAsync(ct);
					_sessions[remoteIp] = (session, runTask);

					// 세션 종료 후 dict 에서 제거 (동일 세션일 때만 — race 방지)
					_ = runTask.ContinueWith(_ =>
					{
						lock (_sessLock)
						{
							if (_sessions.TryGetValue(remoteIp, out var cur) &&
							    ReferenceEquals(cur.Session, session))
								_sessions.Remove(remoteIp);
						}
					}, TaskContinuationOptions.ExecuteSynchronously);
				}
			}
			catch (OperationCanceledException)
			{
				break;
			}
			catch (Exception ex)
			{
				_log.LogError(ex, "수락 오류");
			}
		}

		lock (_sessLock)
		{
			foreach (var (s, _) in _sessions.Values)
				s.Dispose();
			_sessions.Clear();
		}

		listener.Stop();
	}
}
