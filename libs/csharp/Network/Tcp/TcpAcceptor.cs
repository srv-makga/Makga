using System.Net;
using System.Net.Sockets;

namespace Makga.Network.Tcp;

/// <summary>
/// Server-side TCP acceptor. Listens on a port and hands off accepted
/// sockets to the provided <see cref="TcpService"/>.
/// Equivalent to C++ IocpAcceptor / RioAcceptor.
///
/// Usage:
///   var acceptor = new TcpAcceptor(service, new IPEndPoint(IPAddress.Any, 7777));
///   acceptor.Start();
///   ...
///   acceptor.Stop();
/// </summary>
public sealed class TcpAcceptor : IDisposable
{
	private readonly TcpService _service;
	private readonly IPEndPoint _endPoint;
	private Socket? _listenSocket;
	private CancellationTokenSource? _cts;

	public TcpAcceptor(TcpService service, IPEndPoint endPoint)
	{
		_service = service;
		_endPoint = endPoint;
	}

	/// <summary>Bind, listen, and begin accepting connections asynchronously.</summary>
	public void Start()
	{
		// Use the transport's factory to create the listen socket.
		// RioTransport will create it with WSA_FLAG_REGISTERED_IO so accepted
		// sockets inherit RIO capability.
		_listenSocket = _service.Transport.CreateSocket();
		_listenSocket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
		_listenSocket.Bind(_endPoint);
		_listenSocket.Listen(backlog: 128);

		_cts = new CancellationTokenSource();
		_ = AcceptLoopAsync(_cts.Token);
	}

	/// <summary>Stop accepting new connections. Existing sessions are unaffected.</summary>
	public void Stop()
	{
		_cts?.Cancel();
		_listenSocket?.Close();
	}

	private async Task AcceptLoopAsync(CancellationToken ct)
	{
		while (!ct.IsCancellationRequested)
		{
			try
			{
				var socket = await _service.Transport.AcceptAsync(_listenSocket!, ct);
				var session = (TcpSession)_service.AllocSession();
				session.Initialize(_service);
				_service.RegisterSession(session);
				session.Start(socket);
			}
			catch (OperationCanceledException) { break; }
			catch (SocketException) { break; }
		}
	}

	public void Dispose() => Stop();
}
