using System.Net;
using System.Net.Sockets;

namespace Makga.Network.Transport;

/// <summary>
/// Pluggable I/O transport abstraction.
/// Swap IOCP, epoll, RIO, or io_uring without changing session logic.
/// </summary>
public interface INetTransport : IDisposable
{
	/// <summary>Identifies the underlying I/O mechanism (e.g. "IOCP", "epoll", "io_uring").</summary>
	string Name { get; }

	void Start();
	void Stop();

	/// <summary>Accept the next incoming TCP connection.</summary>
	ValueTask<Socket> AcceptAsync(Socket listenSocket, CancellationToken ct);

	/// <summary>Connect to a remote endpoint.</summary>
	ValueTask ConnectAsync(Socket socket, EndPoint endpoint, CancellationToken ct);

	/// <summary>
	/// Read bytes from <paramref name="socket"/> into <paramref name="buffer"/>.
	/// Returns 0 on graceful close.
	/// </summary>
	ValueTask<int> ReceiveAsync(Socket socket, Memory<byte> buffer, CancellationToken ct);

	/// <summary>Write all of <paramref name="data"/> to <paramref name="socket"/>.</summary>
	ValueTask SendAsync(Socket socket, ReadOnlyMemory<byte> data, CancellationToken ct);

	/// <summary>
	/// Create a socket suitable for this transport.
	/// RioTransport overrides this to apply WSA_FLAG_REGISTERED_IO.
	/// </summary>
	Socket CreateSocket(
		AddressFamily af = AddressFamily.InterNetwork,
		SocketType st = SocketType.Stream,
		ProtocolType pt = ProtocolType.Tcp)
		=> new Socket(af, st, pt);
}
