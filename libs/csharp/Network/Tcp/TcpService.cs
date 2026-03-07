using System.Collections.Concurrent;
using Makga.Network.Core;
using Makga.Network.Transport;

namespace Makga.Network.Tcp;

/// <summary>
/// Abstract TCP service that manages the lifetime of all active sessions.
/// Derive and override <see cref="AllocSession"/> to supply application sessions.
/// Equivalent to C++ IocpService / RioService.
///
/// Usage:
///   var service  = new MyGameService();
///   var acceptor = new TcpAcceptor(service, new IPEndPoint(IPAddress.Any, 7777));
///   acceptor.Start();
///
/// Transport selection (optional):
///   new MyGameService()                          — auto-selects best (io_uring / IOCP / epoll)
///   new MyGameService(TransportFactory.CreateSaea()) — force IOCP / epoll
/// </summary>
public abstract class TcpService : IDisposable
{
	private readonly ConcurrentDictionary<SessionId, TcpSession> _sessions = new();

	/// <summary>The I/O transport used by all sessions of this service.</summary>
	public INetTransport Transport { get; }

	protected TcpService(INetTransport? transport = null)
	{
		// TransportFactory.CreateBest() creates AND starts the transport.
		// User-provided transport is assumed already started; Start() is idempotent.
		Transport = transport ?? TransportFactory.CreateBest();
		Transport.Start();
	}

	// ─── Overridable factory / hooks ────────────────────────────────────────

	/// <summary>Create a new session instance. Called for every accepted/connected socket.</summary>
	public abstract TcpSession AllocSession();

	/// <summary>Called after a session connects and OnConnected has run.</summary>
	public virtual void OnSessionConnected(TcpSession session) { }

	/// <summary>Called after a session disconnects and OnDisconnected has run.</summary>
	public virtual void OnSessionDisconnected(TcpSession session) { }

	// ─── Session registry ───────────────────────────────────────────────────

	/// <summary>Number of currently active sessions.</summary>
	public int SessionCount => _sessions.Count;

	internal void RegisterSession(TcpSession session)
	{
		_sessions[session.Id] = session;
		OnSessionConnected(session);
	}

	internal void UnregisterSession(NetSession session)
	{
		_sessions.TryRemove(session.Id, out _);
	}

	internal void NotifyDisconnected(TcpSession session)
	{
		OnSessionDisconnected(session);
	}

	// ─── Broadcast helpers ──────────────────────────────────────────────────

	/// <summary>Send <paramref name="data"/> to every active session.</summary>
	public void BroadcastAll(ReadOnlyMemory<byte> data)
	{
		foreach (var (_, session) in _sessions)
		{
			session.Send(data);
		}
	}

	/// <summary>Disconnect all active sessions.</summary>
	public void DisconnectAll()
	{
		foreach (var (_, session) in _sessions)
		{
			session.Disconnect();
		}
	}

	public void Dispose()
	{
		DisconnectAll();
		Transport.Dispose();
	}
}
