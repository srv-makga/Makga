#if OS_WINDOWS
using System.Collections.Concurrent;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;

namespace Makga.Network.Transport;

/// <summary>
/// High-performance transport using Windows Registered I/O (RIO).
/// RIO bypasses the standard IOCP path — the completion queue is polled
/// directly in user-space, eliminating kernel-transition overhead per I/O op.
///
/// Requires: Windows 8 / Server 2012 or later.
/// Equivalent to C++ RioCore + RioSession.
///
/// Architecture:
///   ReceiveAsync / SendAsync → register buffer → post to RIO_RQ
///   _completionThread        → RIODequeueCompletion → complete TCS
/// </summary>
[SupportedOSPlatform("windows")]
public sealed class RioTransport : INetTransport
{
	private const uint CqSize = 4096;
	private const uint RqMaxRecv = 128;
	private const uint RqMaxSend = 128;

	private RioFunctions? _rio;
	private IntPtr _cq;
	private Thread? _completionThread;
	private volatile bool _running;

	// socket handle (nint) → RIO_RQ (IntPtr)
	private readonly ConcurrentDictionary<nint, IntPtr> _requestQueues = new();
	private readonly ConcurrentDictionary<ulong, PendingOp> _pending = new();
	private ulong _nextId;

	public string Name => "RIO";

	// ─── Lifecycle ───────────────────────────────────────────────────────────

	public void Start()
	{
		if (_running) { return; }  // idempotent

		_rio = LoadRioFunctions();
		_cq = _rio.CreateCQ(CqSize);
		if (IntPtr.Zero == _cq)
		{
			throw new InvalidOperationException($"RIOCreateCompletionQueue failed: {Marshal.GetLastPInvokeError()}");
		}

		_running = true;
		_completionThread = new Thread(CompletionLoop)
		{
			IsBackground = true,
			Name = "RIO-CQ",
			Priority = ThreadPriority.AboveNormal
		};
		_completionThread.Start();
	}

	public void Stop()
	{
		_running = false;
		_completionThread?.Join(TimeSpan.FromSeconds(1));
		if (IntPtr.Zero != _cq) { _rio?.CloseCQ(_cq); _cq = IntPtr.Zero; }
	}

	public void Dispose() => Stop();

	// ─── Socket creation ─────────────────────────────────────────────────────

	/// <summary>Create a socket with WSA_FLAG_REGISTERED_IO so it can use RIO.</summary>
	public Socket CreateSocket(
		AddressFamily af = AddressFamily.InterNetwork,
		SocketType st = SocketType.Stream,
		ProtocolType pt = ProtocolType.Tcp)
	{
		var handle = RioNative.WSASocket(
			(int)af, (int)st, (int)pt,
			IntPtr.Zero, 0,
			RioNative.WSA_FLAG_OVERLAPPED | RioNative.WSA_FLAG_REGISTERED_IO);

		if (RioNative.INVALID_SOCKET == handle)
		{
			throw new SocketException(RioNative.WSAGetLastError());
		}
		return new Socket(new SafeSocketHandle(handle, ownsHandle: true));
	}

	// ─── Per-socket RIO request queue ────────────────────────────────────────

	private IntPtr GetOrCreateRQ(Socket socket)
	{
		return _requestQueues.GetOrAdd((nint)socket.SafeHandle.DangerousGetHandle(), handle =>
		{
			var rq = _rio!.CreateRQ((IntPtr)handle, RqMaxRecv, RqMaxSend, _cq);
			if (IntPtr.Zero == rq)
			{
				throw new InvalidOperationException(
					$"RIOCreateRequestQueue failed: {Marshal.GetLastPInvokeError()}. " +
					$"Ensure the listen socket was created via RioTransport.CreateSocket().");
			}
			return rq;
		});
	}

	// ─── Completion loop (dedicated polling thread) ──────────────────────────

	private unsafe void CompletionLoop()
	{
		var results = new RIORESULT[CqSize];
		fixed (RIORESULT* pResults = results)
		{
			while (_running)
			{
				var count = _rio!.DequeueCQ(_cq, pResults, CqSize);
				if (0 == count)
				{
					Thread.SpinWait(100);
					continue;
				}

				for (var i = 0; i < (int)count; i++)
				{
					var r = results[i];
					if (!_pending.TryRemove(r.RequestContext, out var op)) { continue; }

					op.Cleanup(_rio!);

					if (0 == r.Status)
					{
						op.Tcs.TrySetResult((int)r.BytesTransferred);
					}
					else
					{
						op.Tcs.TrySetException(new SocketException(r.Status));
					}
				}
			}
		}
	}

	// ─── Accept / Connect (standard socket API — RIO only used for data) ─────

	public ValueTask<Socket> AcceptAsync(Socket listenSocket, CancellationToken ct) =>
		listenSocket.AcceptAsync(ct);

	public ValueTask ConnectAsync(Socket socket, EndPoint endpoint, CancellationToken ct) =>
		socket.ConnectAsync(endpoint, ct);

	// ─── Receive ─────────────────────────────────────────────────────────────

	public unsafe ValueTask<int> ReceiveAsync(Socket socket, Memory<byte> buffer, CancellationToken ct)
	{
		var id = Interlocked.Increment(ref _nextId);
		var tcs = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

		MemoryMarshal.TryGetArray((ReadOnlyMemory<byte>)buffer, out var seg);
		var pin = GCHandle.Alloc(seg.Array!, GCHandleType.Pinned);
		var ptr = pin.AddrOfPinnedObject() + seg.Offset;

		var rioBuf = new RIO_BUF
		{
			BufferId = _rio!.RegisterBuffer(ptr, (uint)seg.Count),
			Offset   = 0,
			Length   = (uint)seg.Count
		};

		_pending[id] = new PendingOp(tcs, pin, rioBuf.BufferId);
		ct.Register(CancelOp, (_pending, id));

		_rio.Receive(GetOrCreateRQ(socket), ref rioBuf, flags: 0, ctx: id);
		return new ValueTask<int>(tcs.Task);
	}

	// ─── Send ────────────────────────────────────────────────────────────────

	public unsafe ValueTask SendAsync(Socket socket, ReadOnlyMemory<byte> data, CancellationToken ct)
	{
		var id = Interlocked.Increment(ref _nextId);
		var tcs = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

		MemoryMarshal.TryGetArray<byte>(data, out var seg);
		var pin = GCHandle.Alloc(seg.Array!, GCHandleType.Pinned);
		var ptr = pin.AddrOfPinnedObject() + seg.Offset;

		var rioBuf = new RIO_BUF
		{
			BufferId = _rio!.RegisterBuffer(ptr, (uint)seg.Count),
			Offset   = 0,
			Length   = (uint)seg.Count
		};

		_pending[id] = new PendingOp(tcs, pin, rioBuf.BufferId);

		_rio.Send(GetOrCreateRQ(socket), ref rioBuf, flags: 0, ctx: id);
		return new ValueTask(tcs.Task.ContinueWith(_ => { }, ct, TaskContinuationOptions.ExecuteSynchronously, TaskScheduler.Default));
	}

	// ─── Helpers ─────────────────────────────────────────────────────────────

	private static void CancelOp(object? state)
	{
		var (dict, id) = ((ConcurrentDictionary<ulong, PendingOp>, ulong))state!;
		if (dict.TryRemove(id, out var op))
		{
			op.Tcs.TrySetCanceled();
			// Note: buffer/pin are intentionally not cleaned here because the
			// kernel may still be using them. They will leak on cancel.
			// Production code should use RIOCancel when available.
		}
	}

	// ─── Private RIO function loader ─────────────────────────────────────────

	private static RioFunctions LoadRioFunctions()
	{
		var sock = RioNative.WSASocket(2, 1, 6, IntPtr.Zero, 0, RioNative.WSA_FLAG_OVERLAPPED);
		if (RioNative.INVALID_SOCKET == sock) { throw new InvalidOperationException("WSASocket failed"); }

		try
		{
			var guid = RioNative.WSAID_MULTIPLE_RIO;
			var tableSize = Marshal.SizeOf<RIO_EXTENSION_FUNCTION_TABLE>();
			var tablePtr = Marshal.AllocHGlobal(tableSize);
			try
			{
				var ret = RioNative.WSAIoctl(
					sock,
					RioNative.SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER,
					ref guid, Marshal.SizeOf<Guid>(),
					tablePtr, tableSize,
					out _, IntPtr.Zero, IntPtr.Zero);

				if (0 != ret)
				{
					throw new InvalidOperationException($"RIO not supported on this Windows version (WSAIoctl error {RioNative.WSAGetLastError()})");
				}

				var table = Marshal.PtrToStructure<RIO_EXTENSION_FUNCTION_TABLE>(tablePtr);
				return new RioFunctions(in table);
			}
			finally
			{
				Marshal.FreeHGlobal(tablePtr);
			}
		}
		finally
		{
			RioNative.closesocket(sock);
		}
	}

	// ─── PendingOp ───────────────────────────────────────────────────────────

	private readonly record struct PendingOp(TaskCompletionSource<int> Tcs, GCHandle Pin, IntPtr BufferId)
	{
		public void Cleanup(RioFunctions rio)
		{
			if (IntPtr.Zero != BufferId) { rio.DeregisterBuffer(BufferId); }
			if (Pin.IsAllocated) { Pin.Free(); }
		}
	}
}

// ─── RIO P/Invoke / structs / delegates ──────────────────────────────────────

[StructLayout(LayoutKind.Sequential)]
internal struct RIO_BUF
{
	public IntPtr BufferId;
	public uint   Offset;
	public uint   Length;
}

[StructLayout(LayoutKind.Sequential)]
internal struct RIORESULT
{
	public int    Status;
	public uint   BytesTransferred;
	public ulong  SocketContext;
	public ulong  RequestContext;
}

[StructLayout(LayoutKind.Sequential)]
internal struct RIO_EXTENSION_FUNCTION_TABLE
{
	public uint  cbSize;
	public IntPtr RIOReceive;
	public IntPtr RIOReceiveEx;
	public IntPtr RIOSend;
	public IntPtr RIOSendEx;
	public IntPtr RIOCloseCompletionQueue;
	public IntPtr RIOCreateCompletionQueue;
	public IntPtr RIOCreateRequestQueue;
	public IntPtr RIODequeueCompletion;
	public IntPtr RIODeregisterBuffer;
	public IntPtr RIONotify;
	public IntPtr RIORegisterBuffer;
	public IntPtr RIOResizeCompletionQueue;
	public IntPtr RIOResizeRequestQueue;
}

[UnmanagedFunctionPointer(CallingConvention.StdCall, SetLastError = true)]
internal unsafe delegate bool RIOReceiveDelegate(IntPtr rq, RIO_BUF* pData, uint count, uint flags, ulong ctx);

[UnmanagedFunctionPointer(CallingConvention.StdCall, SetLastError = true)]
internal unsafe delegate bool RIOSendDelegate(IntPtr rq, RIO_BUF* pData, uint count, uint flags, ulong ctx);

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
internal delegate IntPtr RIOCreateCQDelegate(uint size, IntPtr notification);

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
internal delegate IntPtr RIOCreateRQDelegate(IntPtr socket, uint maxRcv, uint maxRcvBufs, uint maxSnd, uint maxSndBufs, IntPtr rcvCQ, IntPtr sndCQ, ulong ctx);

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
internal unsafe delegate uint RIODequeueCQDelegate(IntPtr cq, RIORESULT* array, uint size);

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
internal delegate IntPtr RIORegisterBufferDelegate(IntPtr dataBuffer, uint dataLength);

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
internal delegate void RIODeregisterBufferDelegate(IntPtr bufferId);

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
internal delegate void RIOCloseCQDelegate(IntPtr cq);

// ─── Wrappers around function-pointer calls ──────────────────────────────────

internal sealed class RioFunctions
{
	private readonly RIOReceiveDelegate        _receive;
	private readonly RIOSendDelegate           _send;
	private readonly RIOCreateCQDelegate       _createCQ;
	private readonly RIOCreateRQDelegate       _createRQ;
	private readonly RIODequeueCQDelegate      _dequeueCQ;
	private readonly RIORegisterBufferDelegate _registerBuf;
	private readonly RIODeregisterBufferDelegate _deregisterBuf;
	private readonly RIOCloseCQDelegate        _closeCQ;

	internal RioFunctions(in RIO_EXTENSION_FUNCTION_TABLE t)
	{
		_receive       = Marshal.GetDelegateForFunctionPointer<RIOReceiveDelegate>(t.RIOReceive);
		_send          = Marshal.GetDelegateForFunctionPointer<RIOSendDelegate>(t.RIOSend);
		_createCQ      = Marshal.GetDelegateForFunctionPointer<RIOCreateCQDelegate>(t.RIOCreateCompletionQueue);
		_createRQ      = Marshal.GetDelegateForFunctionPointer<RIOCreateRQDelegate>(t.RIOCreateRequestQueue);
		_dequeueCQ     = Marshal.GetDelegateForFunctionPointer<RIODequeueCQDelegate>(t.RIODequeueCompletion);
		_registerBuf   = Marshal.GetDelegateForFunctionPointer<RIORegisterBufferDelegate>(t.RIORegisterBuffer);
		_deregisterBuf = Marshal.GetDelegateForFunctionPointer<RIODeregisterBufferDelegate>(t.RIODeregisterBuffer);
		_closeCQ       = Marshal.GetDelegateForFunctionPointer<RIOCloseCQDelegate>(t.RIOCloseCompletionQueue);
	}

	internal unsafe bool Receive(IntPtr rq, ref RIO_BUF buf, uint flags, ulong ctx)
	{
		fixed (RIO_BUF* p = &buf) { return _receive(rq, p, 1, flags, ctx); }
	}

	internal unsafe bool Send(IntPtr rq, ref RIO_BUF buf, uint flags, ulong ctx)
	{
		fixed (RIO_BUF* p = &buf) { return _send(rq, p, 1, flags, ctx); }
	}

	internal IntPtr CreateCQ(uint size) => _createCQ(size, IntPtr.Zero);

	internal IntPtr CreateRQ(IntPtr socket, uint maxRcv, uint maxSnd, IntPtr cq) =>
		_createRQ(socket, maxRcv, 1, maxSnd, 1, cq, cq, (ulong)socket);

	internal unsafe uint DequeueCQ(IntPtr cq, RIORESULT* arr, uint size) =>
		_dequeueCQ(cq, arr, size);

	internal IntPtr RegisterBuffer(IntPtr ptr, uint len) => _registerBuf(ptr, len);
	internal void DeregisterBuffer(IntPtr id) => _deregisterBuf(id);
	internal void CloseCQ(IntPtr cq) => _closeCQ(cq);
}

// ─── Native declarations ─────────────────────────────────────────────────────

internal static class RioNative
{
	internal const uint WSA_FLAG_OVERLAPPED        = 0x01;
	internal const uint WSA_FLAG_REGISTERED_IO     = 0x100;
	internal const uint SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER = 0xC8000026;
	internal static readonly IntPtr INVALID_SOCKET = new(-1);

	// {8509E081-96DD-4005-B165-9E2EE8C79E3F}
	internal static readonly Guid WSAID_MULTIPLE_RIO =
		new(0x8509e081, 0x96dd, 0x4005, 0xb1, 0x65, 0x9e, 0x2e, 0xe8, 0xc7, 0x9e, 0x3f);

	[DllImport("Ws2_32.dll", SetLastError = true)]
	internal static extern IntPtr WSASocket(int af, int type, int proto,
		IntPtr protocolInfo, uint group, uint flags);

	[DllImport("Ws2_32.dll", SetLastError = true)]
	internal static extern int WSAIoctl(IntPtr s, uint code,
		ref Guid inBuf, int inLen,
		IntPtr outBuf, int outLen,
		out int bytesReturned, IntPtr overlapped, IntPtr completionRoutine);

	[DllImport("Ws2_32.dll")]
	internal static extern int closesocket(IntPtr s);

	[DllImport("Ws2_32.dll")]
	internal static extern int WSAGetLastError();
}
#endif
