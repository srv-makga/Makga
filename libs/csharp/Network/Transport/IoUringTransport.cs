#if OS_LINUX
using System.Collections.Concurrent;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;
using IoUring;

namespace Makga.Network.Transport;

/// <summary>
/// High-performance transport using Linux io_uring (kernel 5.1+).
/// A single shared Ring batches all socket I/O across every session,
/// eliminating per-syscall overhead compared to epoll.
///
/// Requires: Linux kernel ≥ 5.1, IoUring NuGet package (tkp1n/IoUring).
///
/// Architecture:
///   Sessions call ReceiveAsync / SendAsync → enqueue to _ring SQ
///   _completionThread polls CQ → completes TaskCompletionSource per op
/// </summary>
[SupportedOSPlatform("linux")]
public sealed class IoUringTransport : INetTransport
{
	private const int RingEntries = 4096;

	private Ring? _ring;
	private Thread? _completionThread;
	private volatile bool _running;

	// Maps userData (op id) → (completion source, pinned buffer handle)
	private readonly ConcurrentDictionary<ulong, PendingOp> _pending = new();
	private ulong _nextId;

	public string Name => "io_uring";

	// ─── Lifecycle ───────────────────────────────────────────────────────────

	private readonly object _lock = new();

	public void Start()
	{
		lock (_lock)
		{
			if (_running) { return; }  // idempotent
			_ring = new Ring(RingEntries);
			_running = true;
		}
		_completionThread = new Thread(CompletionLoop)
		{
			IsBackground = true,
			Name = "IoUring-CQ"
		};
		_completionThread.Start();
	}

	public void Stop()
	{
		_running = false;
		_ring?.Dispose();
		_completionThread?.Join(timeout: 1000);
	}

	public void Dispose() => Stop();

	// ─── Completion loop (dedicated thread) ──────────────────────────────────

	private void CompletionLoop()
	{
		while (_running)
		{
			try
			{
				// Block until at least one CQE is ready, then drain the queue
				_ring!.SubmitAndWait(minComplete: 1);

				while (_ring.TryRead(out var completion))
				{
					if (!_pending.TryRemove(completion.UserData, out var op)) { continue; }

					op.Unpin();

					if (completion.Result >= 0)
					{
						op.Tcs.TrySetResult(completion.Result);
					}
					else
					{
						op.Tcs.TrySetException(new SocketException(-completion.Result));
					}
				}
			}
			catch (Exception ex) when (_running)
			{
				// Log and continue — do not crash the completion thread
				System.Diagnostics.Debug.Fail($"IoUring CQ error: {ex.Message}");
			}
		}
	}

	// ─── Accept / Connect ────────────────────────────────────────────────────

	// Accept and Connect use the standard Socket API; io_uring's advantage is
	// in bulk data transfer, and accept/connect are infrequent by comparison.
	public ValueTask<Socket> AcceptAsync(Socket listenSocket, CancellationToken ct) =>
		listenSocket.AcceptAsync(ct);

	public ValueTask ConnectAsync(Socket socket, EndPoint endpoint, CancellationToken ct) =>
		socket.ConnectAsync(endpoint, ct);

	// ─── Receive ─────────────────────────────────────────────────────────────

	public ValueTask<int> ReceiveAsync(Socket socket, Memory<byte> buffer, CancellationToken ct)
	{
		var id = Interlocked.Increment(ref _nextId);
		var tcs = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

		// Pin buffer so the GC cannot move it while io_uring holds a pointer
		MemoryMarshal.TryGetArray(buffer, out var segment);
		var pin = GCHandle.Alloc(segment.Array!, GCHandleType.Pinned);
		var span = segment.Array!.AsSpan(segment.Offset, segment.Count);

		var op = new PendingOp(tcs, pin);
		_pending[id] = op;

		ct.Register(state =>
		{
			var (pending, opId) = ((ConcurrentDictionary<ulong, PendingOp>, ulong))state!;
			if (pending.TryRemove(opId, out var cancelled))
			{
				cancelled.Unpin();
				cancelled.Tcs.TrySetCanceled();
			}
		}, (_pending, id));

		var fd = (int)socket.SafeHandle.DangerousGetHandle();

		if (!_ring!.TryPrepareRecv(fd, span, SocketFlags.None, userData: id))
		{
			// Submission queue full — flush first, then retry
			_ring.Submit();
			_ring.TryPrepareRecv(fd, span, SocketFlags.None, userData: id);
		}

		return new ValueTask<int>(tcs.Task);
	}

	// ─── Send ────────────────────────────────────────────────────────────────

	public ValueTask SendAsync(Socket socket, ReadOnlyMemory<byte> data, CancellationToken ct)
	{
		var id = Interlocked.Increment(ref _nextId);
		var tcs = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

		MemoryMarshal.TryGetArray(data, out var segment);
		var pin = GCHandle.Alloc(segment.Array!, GCHandleType.Pinned);
		var span = new ReadOnlySpan<byte>(segment.Array!, segment.Offset, segment.Count);

		_pending[id] = new PendingOp(tcs, pin);

		var fd = (int)socket.SafeHandle.DangerousGetHandle();

		if (!_ring!.TryPrepareSend(fd, span, SocketFlags.None, userData: id))
		{
			_ring.Submit();
			_ring.TryPrepareSend(fd, span, SocketFlags.None, userData: id);
		}

		return new ValueTask(tcs.Task.ContinueWith(_ => { }, ct));
	}

	// ─── Internal op handle ──────────────────────────────────────────────────

	private readonly record struct PendingOp(TaskCompletionSource<int> Tcs, GCHandle Pin)
	{
		public void Unpin()
		{
			if (Pin.IsAllocated) { Pin.Free(); }
		}
	}
}
#endif
