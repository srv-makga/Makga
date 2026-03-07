using System.Buffers;
using System.IO.Pipelines;
using System.Net;
using System.Net.Sockets;
using System.Threading.Channels;
using Makga.Network.Core;
using Makga.Network.Transport;

namespace Makga.Network.Tcp;

/// <summary>
/// High-performance TCP session using System.IO.Pipelines for recv buffering
/// and Channel&lt;T&gt; for ordered send queuing.
/// I/O is delegated to an injected <see cref="INetTransport"/>:
///   Windows → IOCP (SaeaTransport)
///   Linux   → io_uring (IoUringTransport) or epoll (SaeaTransport fallback)
///
/// Architecture:
///   transport.ReceiveAsync  →  Pipe.Writer  →  [RunPipeReaderAsync]  →  OnReceived()
///   Send(data)              →  _sendQueue   →  [RunSendLoopAsync]    →  transport.SendAsync
///
/// Equivalent to C++ IocpSession / RioSession.
/// </summary>
public abstract class TcpSession : NetSession
{
	private Socket? _socket;
	private TcpService? _service;
	private INetTransport? _transport;
	private readonly Pipe _recvPipe = new();
	private readonly Channel<ReadOnlyMemory<byte>> _sendQueue =
		Channel.CreateUnbounded<ReadOnlyMemory<byte>>(new UnboundedChannelOptions { SingleReader = true });
	private CancellationTokenSource? _cts;
	private int _disconnected;  // 0 = connected, 1 = disconnecting

	// ─── Internal startup (called by acceptor / connector) ──────────────────

	internal void Initialize(TcpService service)
	{
		_service = service;
		_transport = service.Transport;
	}

	internal override void Start(Socket socket)
	{
		_socket = socket;
		_socket.NoDelay = true;
		RemoteEndPoint = socket.RemoteEndPoint as IPEndPoint;
		_cts = new CancellationTokenSource();

		OnConnected();

		_ = RunReceiveLoopAsync(_cts.Token);   // socket → pipe.Writer  (via transport)
		_ = RunPipeReaderAsync(_cts.Token);    // pipe.Reader → OnReceived
		_ = RunSendLoopAsync(_cts.Token);      // sendQueue → socket    (via transport)
	}

	// ─── Public API ─────────────────────────────────────────────────────────

	/// <summary>Enqueue data for sending. Thread-safe. Non-blocking.</summary>
	public override void Send(ReadOnlyMemory<byte> data) => _sendQueue.Writer.TryWrite(data);

	/// <summary>Initiate graceful disconnect. Idempotent.</summary>
	public override void Disconnect()
	{
		if (1 == Interlocked.Exchange(ref _disconnected, 1)) { return; }

		try { _socket?.Shutdown(SocketShutdown.Both); } catch { }
		_socket?.Close();
		_cts?.Cancel();
		_recvPipe.Writer.Complete();
		_sendQueue.Writer.Complete();

		_service?.UnregisterSession(this);
		_service?.NotifyDisconnected(this);
		OnDisconnected();
	}

	// ─── Recv pipeline ──────────────────────────────────────────────────────

	private async Task RunReceiveLoopAsync(CancellationToken ct)
	{
		var writer = _recvPipe.Writer;
		try
		{
			while (true)
			{
				var buffer = writer.GetMemory(4096);
				var received = await _transport!.ReceiveAsync(_socket!, buffer, ct);
				if (0 == received) { break; }  // graceful close by remote

				writer.Advance(received);
				var flush = await writer.FlushAsync(ct);
				if (flush.IsCompleted) { break; }
			}
		}
		catch (OperationCanceledException) { }
		catch (SocketException) { }
		finally
		{
			await writer.CompleteAsync();
		}
	}

	private async Task RunPipeReaderAsync(CancellationToken ct)
	{
		var reader = _recvPipe.Reader;
		try
		{
			while (true)
			{
				var result = await reader.ReadAsync(ct);
				var buffer = result.Buffer;

				if (!buffer.IsEmpty)
				{
					OnReceived(buffer, out var consumed);
					reader.AdvanceTo(buffer.GetPosition(consumed));
				}

				if (result.IsCompleted) { break; }
			}
		}
		catch (OperationCanceledException) { }
		finally
		{
			reader.Complete();
			Disconnect();
		}
	}

	// ─── Send pipeline ──────────────────────────────────────────────────────

	private async Task RunSendLoopAsync(CancellationToken ct)
	{
		try
		{
			await foreach (var data in _sendQueue.Reader.ReadAllAsync(ct))
			{
				if (null == _socket) { break; }
				await _transport!.SendAsync(_socket, data, ct);
			}
		}
		catch (OperationCanceledException) { }
		catch (SocketException) { Disconnect(); }
	}
}
