using System.Buffers;
using System.Net;
using System.Net.Sockets;

namespace Makga.Network.Core;

/// <summary>
/// Abstract base class for all network sessions.
/// Equivalent to C++ NetSession.
/// Derived classes implement the three lifecycle callbacks and are managed
/// by a <see cref="Tcp.TcpService"/>.
/// </summary>
public abstract class NetSession
{
	public SessionId Id { get; } = SessionId.Next();
	public IPEndPoint? RemoteEndPoint { get; protected set; }

	// ─── Lifecycle callbacks ────────────────────────────────────────────────

	/// <summary>Called once after the connection is established.</summary>
	protected abstract void OnConnected();

	/// <summary>Called once after the connection is fully closed.</summary>
	protected abstract void OnDisconnected();

	/// <summary>
	/// Called when incoming data is available.
	/// Consume what you have fully parsed and set <paramref name="consumed"/>
	/// to the number of bytes processed from the start of <paramref name="buffer"/>.
	/// Unprocessed bytes are retained for the next call.
	/// </summary>
	protected abstract void OnReceived(ReadOnlySequence<byte> buffer, out long consumed);

	// ─── Internal startup (called by acceptor / connector / service) ───────

	internal abstract void Start(Socket socket);

	/// <summary>Enqueue data for sending. Thread-safe.</summary>
	public abstract void Send(ReadOnlyMemory<byte> data);

	/// <summary>Initiate graceful disconnect. Must be idempotent.</summary>
	public abstract void Disconnect();
}
