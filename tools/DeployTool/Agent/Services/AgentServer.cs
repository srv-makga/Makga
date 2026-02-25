using System.Net;
using System.Net.Sockets;
using DeployTool.Protocol.Net;
using DeployTool.Protocol.Packets;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace DeployTool.Agent.Services;

public class AgentServer : BackgroundService
{
	private readonly AgentConfig        _cfg;
	private readonly CommandDispatcher  _dispatcher;
	private readonly ILogger<AgentServer> _log;

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
				var client = await listener.AcceptTcpClientAsync(ct);
				_ = HandleClientAsync(client, ct);
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

		listener.Stop();
	}

	private async Task HandleClientAsync(TcpClient client, CancellationToken ct)
	{
		var remote = client.Client.RemoteEndPoint?.ToString() ?? "unknown";
		_log.LogInformation("Connection from {Remote}", remote);

		using (client)
		{
			var stream    = client.GetStream();
			var startTime = DateTime.UtcNow;
			var authed    = false;

			try
			{
				while (!ct.IsCancellationRequested)
				{
					var packet = await PacketSerializer.ReadPacketAsync(stream, ct);
					if (null == packet)
						break;

					var (id, payload, _) = packet.Value;

					// 첫 패킷은 반드시 Ping(인증)
					if (!authed)
					{
						if (id != Protocol.PacketId.Ping)
						{
							_log.LogWarning("First packet not Ping from {Remote}", remote);
							break;
						}

						var ping = PacketSerializer.Deserialize<PingRequest>(payload);
						if (null == ping || ping.Token != _cfg.Token)
						{
							_log.LogWarning("Auth failed from {Remote}", remote);
							await stream.WriteAsync(PacketSerializer.Serialize(
								Protocol.PacketId.Result,
								new ResultResponse { Success = false, Message = "Unauthorized", Code = 401 }), ct);
							break;
						}

						authed = true;
						var uptime = (ulong)(DateTime.UtcNow - startTime).TotalSeconds;
						await stream.WriteAsync(PacketSerializer.Serialize(
							Protocol.PacketId.Pong, new PongResponse { UptimeSec = uptime }), ct);
						continue;
					}

					var response = await _dispatcher.DispatchAsync(id, payload, ct);
					if (null != response)
						await stream.WriteAsync(response, ct);
				}
			}
			catch (Exception ex) when (ex is not OperationCanceledException)
			{
				_log.LogError(ex, "Client error {Remote}", remote);
			}
		}

		_log.LogInformation("Disconnected {Remote}", remote);
	}
}
