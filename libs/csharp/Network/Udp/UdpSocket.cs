using System.Net;
using System.Net.Sockets;

namespace Makga.Network.Udp;

/// <summary>
/// Connectionless UDP socket. Fires a callback for every received datagram.
/// Equivalent to C++ UdpSocket.
///
/// Usage:
///   var udp = new UdpSocket((remote, data) => Console.WriteLine($"From {remote}: {data.Length} bytes"));
///   udp.Open(port: 9000);
///   udp.SendTo(remoteEndPoint, payload);
///   ...
///   udp.Close();
/// </summary>
public sealed class UdpSocket : IDisposable
{
	private readonly Action<IPEndPoint, ReadOnlyMemory<byte>> _onMessage;
	private Socket? _socket;
	private CancellationTokenSource? _cts;

	public UdpSocket(Action<IPEndPoint, ReadOnlyMemory<byte>> onMessage)
	{
		_onMessage = onMessage;
	}

	/// <summary>Bind to <paramref name="port"/> and begin receiving datagrams.</summary>
	public void Open(int port)
	{
		_socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
		_socket.Bind(new IPEndPoint(IPAddress.Any, port));
		_cts = new CancellationTokenSource();
		_ = ReceiveLoopAsync(_cts.Token);
	}

	/// <summary>Stop receiving and release the socket.</summary>
	public void Close()
	{
		_cts?.Cancel();
		_socket?.Close();
	}

	/// <summary>Send <paramref name="data"/> to <paramref name="remote"/>. Non-blocking.</summary>
	public void SendTo(IPEndPoint remote, ReadOnlyMemory<byte> data)
	{
		_socket?.SendTo(data.Span, remote);
	}

	private async Task ReceiveLoopAsync(CancellationToken ct)
	{
		// Max UDP datagram size; reused across iterations — no per-message allocation.
		var buffer = new byte[65535];
		EndPoint remoteAny = new IPEndPoint(IPAddress.Any, 0);
		try
		{
			while (!ct.IsCancellationRequested)
			{
				var result = await _socket!.ReceiveFromAsync(
					buffer.AsMemory(), SocketFlags.None, remoteAny, ct);

				var endpoint = (IPEndPoint)result.RemoteEndPoint;
				_onMessage(endpoint, buffer.AsMemory(0, result.ReceivedBytes));
			}
		}
		catch (OperationCanceledException) { }
		catch (SocketException) { }
	}

	public void Dispose() => Close();
}
