using System.Net.Sockets;
using DeployTool.Common.Net;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Logging;

namespace DeployTool.Agent.Services;

/// <summary>
/// 매니저 클라이언트와의 단일 TCP 세션을 관리합니다.
/// Ping/Pong을 통한 인증을 처리하고 패킷 수신 루프를 유지하며 명령을 처리합니다.
/// </summary>
public sealed class AgentSession : IDisposable
{
	private readonly TcpClient          _tcp;
	private readonly CommandDispatcher  _dispatcher;
	private readonly AgentConfig        _cfg;
	private readonly ILogger            _log;
	private readonly DateTime           _agentStartTime;
	private readonly CancellationTokenSource _cts = new();

	/// <summary>
	/// AgentSession 클래스의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="tcp">매니저에 연결된 TCP 클라이언트</param>
	/// <param name="dispatcher">패킷 라우팅을 위한 명령 디스패처</param>
	/// <param name="cfg">에이전트 설정</param>
	/// <param name="log">로거 인스턴스</param>
	/// <param name="agentStartTime">가동 시간 계산을 위한 에이전트의 시작 시간</param>
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

	/// <summary>
	/// 연결이 닫히거나 취소될 때까지 세션 루프를 실행하여 인증 및 패킷 처리를 처리합니다.
	/// </summary>
	/// <param name="externalCt">외부 취소 토큰</param>
	/// <returns>세션 수명 주기를 나타내는 작업</returns>
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

					// ─── 첫 패킷은 반드시 Ping(인증)이어야 합니다 ───────────────
					if (!authed)
					{
						if (id != Common.PacketId.Ping)
						{
							_log.LogWarning("[Session] First packet not Ping from {Remote}", remote);
							break;
						}

						var ping = PacketSerializer.Deserialize<PingRequest>(payload);
						if (null == ping)
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

	/// <summary>
	/// 세션을 취소하여 RunAsync 루프의 강제 종료를 시작합니다.
	/// </summary>
	public void Cancel() => _cts.Cancel();

	/// <summary>
	/// 세션이 사용하는 리소스를 해제하고 세션을 취소하며 TCP 클라이언트를 정리합니다.
	/// </summary>
	public void Dispose()
	{
		_cts.Cancel();
		_cts.Dispose();
		try { _tcp.Dispose(); } catch { }
	}
}
