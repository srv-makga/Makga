using System.Net;
using System.Net.Sockets;

namespace Makga.Network.Tcp;

/// <summary>
/// Client-side TCP connector. Connects to a remote endpoint and registers
/// the resulting session with the provided <see cref="TcpService"/>.
/// Equivalent to C++ IocpConnector / RioConnector.
///
/// Usage:
///   var connector = new TcpConnector(service);
///   var session   = await connector.ConnectAsync(new IPEndPoint(remoteIp, 7777));
/// </summary>
public sealed class TcpConnector
{
	private readonly TcpService _service;

	public TcpConnector(TcpService service)
	{
		_service = service;
	}

	/// <summary>
	/// Asynchronously connect to <paramref name="endpoint"/>.
	/// Returns the connected session on success, or null on failure.
	/// </summary>
	public async Task<TcpSession?> ConnectAsync(IPEndPoint endpoint, CancellationToken ct = default)
	{
		var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
		socket.NoDelay = true;
		try
		{
			await _service.Transport.ConnectAsync(socket, endpoint, ct);

			var session = (TcpSession)_service.AllocSession();
			session.Initialize(_service);
			_service.RegisterSession(session);
			session.Start(socket);
			return session;
		}
		catch (Exception)
		{
			socket.Dispose();
			return null;
		}
	}
}
