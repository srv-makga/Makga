using System.Runtime.InteropServices;
using System.Runtime.Versioning;

namespace Makga.Network.Transport;

/// <summary>
/// Creates the best available transport for the current platform.
///
/// Selection order:
///   Windows → RIO (Win8+)   → IOCP (SaeaTransport fallback)
///   Linux   → io_uring (5.1+) → epoll (SaeaTransport fallback)
///   Other   → SaeaTransport
/// </summary>
public static class TransportFactory
{
	/// <summary>
	/// Create and start the highest-performance transport available.
	/// Falls back gracefully if the preferred transport cannot be initialised.
	/// </summary>
	public static INetTransport CreateBest()
	{
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			var rio = TryCreateAndStart(CreateRio);
			if (null != rio) { return rio; }
		}

		if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
		{
			var uring = TryCreateAndStart(CreateIoUring);
			if (null != uring) { return uring; }
		}

		var saea = new SaeaTransport();
		saea.Start();  // no-op but explicit
		return saea;
	}

	/// <summary>Force IOCP / epoll transport (SaeaTransport). Useful for testing.</summary>
	public static INetTransport CreateSaea()
	{
		var t = new SaeaTransport();
		t.Start();
		return t;
	}

	// ─── Private factory helpers ─────────────────────────────────────────────

	private static INetTransport? TryCreateAndStart(Func<INetTransport?> factory)
	{
		try
		{
			var t = factory();
			if (null == t) { return null; }
			t.Start();
			return t;
		}
		catch
		{
			return null;
		}
	}

	[SupportedOSPlatform("windows")]
	private static INetTransport? CreateRio()
	{
#if OS_WINDOWS
		return new RioTransport();
#else
		return null;
#endif
	}

	[SupportedOSPlatform("linux")]
	private static INetTransport? CreateIoUring()
	{
#if OS_LINUX
		return new IoUringTransport();
#else
		return null;
#endif
	}
}
