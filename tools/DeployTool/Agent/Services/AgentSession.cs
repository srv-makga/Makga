using System.Net.Sockets;
using DeployTool.Common.Net;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Logging;

namespace DeployTool.Agent.Services;

// @brief Manager 와의 단일 TCP 세션을 전담 관리.
//   - 인증(Ping/Pong) 후 패킷 수신 루프를 유지
//   - 연결이 끊기거나 Cancel() 이 호출되면 RunAsync 가 완료된다.
//   - AgentServer 는 Listen 과 세션 교체만 담당하고 실제 통신은 이 클래스에 위임
public sealed class AgentSession : IDisposable
{
	private readonly TcpClient          _tcp;
	private readonly CommandDispatcher  _dispatcher;
	private readonly AgentConfig        _cfg;
	private readonly ILogger            _log;
	private readonly DateTime           _agentStartTime;
	private readonly CancellationTokenSource _cts = new();

	public AgentSession(
		TcpClient tcp, CommandDispatcher dispatcher,
		AgentConfig cfg, ILogger log, DateTime agentStartTime)
	{
		_tcp            = tcp;
		_dispatcher     = dispatcher;
		_cfg            = cfg;
		_log            = log;
		_agentStartTime = agentStartTime;
	}

	// @brief 연결이 끊기거나 외부에서 취소될 때까지 블로킹
	public async Task RunAsync(CancellationToken externalCt)
	{
		using var linked = CancellationTokenSource.CreateLinkedTokenSource(externalCt, _cts.Token);
		var ct     = linked.Token;
		var remote = _tcp.Client.RemoteEndPoint?.ToString() ?? "unknown";
		_log.LogInformation("[Session] Connected: {Remote}", remote);

		using (_tcp)
		{
			var stream = _tcp.GetStream();
			var authed = false;
			try
			{
				while (!ct.IsCancellationRequested)
				{
					var packet = await PacketSerializer.ReadPacketAsync(stream, ct);
					if (null == packet)
						break;

					var (id, payload, _) = packet.Value;

					// ─── 첫 패킷은 반드시 Ping(인증) ───────────────
					if (!authed)
					{
						if (id != Common.PacketId.Ping)
						{
							_log.LogWarning("[Session] First packet not Ping from {Remote}", remote);
							break;
						}

						var ping = PacketSerializer.Deserialize<PingRequest>(payload);
						if (null == ping || ping.Token != _cfg.Token)
						{
							_log.LogWarning("[Session] Auth failed from {Remote}", remote);
							await stream.WriteAsync(PacketSerializer.Serialize(
								Common.PacketId.Result,
								new ResultResponse { Success = false, Message = "Unauthorized", Code = 401 }), ct);
							break;
						}

						authed = true;
						var uptime = (ulong)(DateTime.UtcNow - _agentStartTime).TotalSeconds;
						await stream.WriteAsync(PacketSerializer.Serialize(
							Common.PacketId.Pong, new PongResponse { UptimeSec = uptime }), ct);
						continue;
					}

					// ─── 명령 처리 ───────────────────────────────
					var response = await _dispatcher.DispatchAsync(id, payload, ct);
					if (null != response)
						await stream.WriteAsync(response, ct);
				}
			}
			catch (Exception ex) when (ex is not OperationCanceledException)
			{
				_log.LogError(ex, "[Session] Error [{Remote}]", remote);
			}
		}

		_log.LogInformation("[Session] Disconnected: {Remote}", remote);
	}

	// @brief 외부에서 세션을 강제 종료할 때 호출
	public void Cancel() => _cts.Cancel();

	public void Dispose()
	{
		_cts.Cancel();
		_cts.Dispose();
		try { _tcp.Dispose(); } catch { }
	}
}
