using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Threading.Tasks.Sources;

namespace Makga.Network.Transport;

/// <summary>
/// Transport backed by SocketAsyncEventArgs (SAEA).
/// Uses IOCP on Windows and epoll on Linux via the .NET runtime — no additional setup needed.
/// Pre-pools SAEA awaitables to eliminate per-operation heap allocation on the hot recv/send path.
/// </summary>
public sealed class SaeaTransport : INetTransport
{
	private readonly SaeaPool _recvPool = new();
	private readonly SaeaPool _sendPool = new();

	public string Name =>
		RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? "IOCP" : "epoll";

	public void Start() { }
	public void Stop() { }
	public void Dispose() { }

	// ─── Accept / Connect ────────────────────────────────────────────────────

	public ValueTask<Socket> AcceptAsync(Socket listenSocket, CancellationToken ct) =>
		listenSocket.AcceptAsync(ct);

	public ValueTask ConnectAsync(Socket socket, EndPoint endpoint, CancellationToken ct) =>
		socket.ConnectAsync(endpoint, ct);

	// ─── Recv ────────────────────────────────────────────────────────────────

	public ValueTask<int> ReceiveAsync(Socket socket, Memory<byte> buffer, CancellationToken ct)
	{
		var awaitable = _recvPool.Rent();
		awaitable.Reset();
		awaitable.Saea.SetBuffer(buffer);

		if (!socket.ReceiveAsync(awaitable.Saea))
		{
			// Completed synchronously — return immediately, no allocation
			var result = awaitable.Saea.SocketError == SocketError.Success
				? new ValueTask<int>(awaitable.Saea.BytesTransferred)
				: ValueTask.FromException<int>(new SocketException((int)awaitable.Saea.SocketError));
			_recvPool.Return(awaitable);
			return result;
		}

		return WaitAsync(awaitable, _recvPool, ct);
	}

	// ─── Send ────────────────────────────────────────────────────────────────

	public ValueTask SendAsync(Socket socket, ReadOnlyMemory<byte> data, CancellationToken ct)
	{
		var awaitable = _sendPool.Rent();
		awaitable.Reset();
		awaitable.Saea.SetBuffer(MemoryMarshal.AsMemory(data));

		if (!socket.SendAsync(awaitable.Saea))
		{
			var err = awaitable.Saea.SocketError;
			_sendPool.Return(awaitable);
			return SocketError.Success != err
				? ValueTask.FromException(new SocketException((int)err))
				: ValueTask.CompletedTask;
		}

		return WaitSendAsync(awaitable, _sendPool, ct);
	}

	// ─── Helpers ─────────────────────────────────────────────────────────────

	private static async ValueTask<int> WaitAsync(SaeaAwaitable a, SaeaPool pool, CancellationToken ct)
	{
		try
		{
			return await a.AsValueTask().AsTask().WaitAsync(ct);
		}
		finally
		{
			pool.Return(a);
		}
	}

	private static async ValueTask WaitSendAsync(SaeaAwaitable a, SaeaPool pool, CancellationToken ct)
	{
		try
		{
			await a.AsValueTask().AsTask().WaitAsync(ct);
		}
		finally
		{
			pool.Return(a);
		}
	}
}

// ─── SAEA pool ───────────────────────────────────────────────────────────────

/// <summary>
/// Lock-free stack of pre-allocated SaeaAwaitable objects.
/// Returns items instantly without allocating; creates new if empty.
/// </summary>
internal sealed class SaeaPool
{
	private SaeaAwaitable? _head;

	public SaeaAwaitable Rent()
	{
		while (true)
		{
			var head = Volatile.Read(ref _head);
			if (null == head) { return new SaeaAwaitable(this); }
			if (null != Interlocked.CompareExchange(ref _head, head.Next, head))
			{
				continue;
			}
			head.Next = null;
			return head;
		}
	}

	public void Return(SaeaAwaitable item)
	{
		while (true)
		{
			var head = Volatile.Read(ref _head);
			item.Next = head;
			if (null == Interlocked.CompareExchange(ref _head, item, head)) { break; }
		}
	}
}

// ─── Zero-alloc SAEA awaitable ───────────────────────────────────────────────

/// <summary>
/// Combines SocketAsyncEventArgs with ManualResetValueTaskSourceCore for
/// zero-allocation async I/O when awaited from a single consumer.
/// </summary>
internal sealed class SaeaAwaitable : IValueTaskSource<int>
{
	private ManualResetValueTaskSourceCore<int> _core;

	internal SaeaAwaitable? Next;  // intrusive stack link for SaeaPool

	public SocketAsyncEventArgs Saea { get; }

	public SaeaAwaitable(SaeaPool? _pool = null)
	{
		Saea = new SocketAsyncEventArgs();
		Saea.Completed += OnCompleted;
	}

	private void OnCompleted(object? _, SocketAsyncEventArgs e)
	{
		if (e.SocketError == SocketError.Success)
		{
			_core.SetResult(e.BytesTransferred);
		}
		else
		{
			_core.SetException(new SocketException((int)e.SocketError));
		}
	}

	public void Reset() => _core.Reset();

	public ValueTask<int> AsValueTask() => new(this, _core.Version);

	int IValueTaskSource<int>.GetResult(short token) => _core.GetResult(token);

	ValueTaskSourceStatus IValueTaskSource<int>.GetStatus(short token) => _core.GetStatus(token);

	void IValueTaskSource<int>.OnCompleted(
		Action<object?> continuation, object? state, short token, ValueTaskSourceOnCompletedFlags flags)
	{
		_core.OnCompleted(continuation, state, token, flags);
	}
}
