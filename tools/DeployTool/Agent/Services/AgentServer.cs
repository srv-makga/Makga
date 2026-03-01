using System.Net;
using System.Net.Sockets;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace DeployTool.Agent.Services;

// @brief TCP Listen + IP별 세션 관리.
//   - 살아있는 세션이 있으면 중복 접속 거절
//   - RunTask.IsCompleted 로 생존 여부 판단 → 죽었으면 교체, 살아있으면 거절
//   - ContinueWith 는 동일 세션 참조일 때만 dict 에서 제거 (교체 후 race 방지)
public class AgentServer : BackgroundService
{
	private readonly AgentConfig          _cfg;
	private readonly CommandDispatcher    _dispatcher;
	private readonly ILogger<AgentServer> _log;
	private readonly DateTime             _startTime = DateTime.UtcNow;

	private readonly Dictionary<string, (AgentSession Session, Task RunTask)> _sessions = new();
	private readonly object _sessLock = new();

	public AgentServer(IOptions<AgentConfig> cfg, CommandDispatcher dispatcher, ILogger<AgentServer> log)
	{
		_cfg        = cfg.Value;
		_dispatcher = dispatcher;
		_log        = log;
	}

	protected override async Task ExecuteAsync(CancellationToken ct)
	{
		var listener = new TcpListener(IPAddress.Any, _cfg.ListenPort);
		listener.Start();
		_log.LogInformation("Agent listening on port {Port}", _cfg.ListenPort);

		while (!ct.IsCancellationRequested)
		{
			try
			{
				var client   = await listener.AcceptTcpClientAsync(ct);
				var remoteIp = ((IPEndPoint)client.Client.RemoteEndPoint!).Address.ToString();

				lock (_sessLock)
				{
					// 살아있는 세션이 있으면 중복 접속 거절
					if (_sessions.TryGetValue(remoteIp, out var existing) && !existing.RunTask.IsCompleted)
					{
						_log.LogWarning("[AgentServer] Rejecting duplicate connection from {IP}", remoteIp);
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
				_log.LogError(ex, "Accept error");
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
