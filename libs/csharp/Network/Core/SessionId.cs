namespace Makga.Network.Core;

/// <summary>
/// Auto-incrementing session identifier. Thread-safe.
/// Equivalent to C++ Id type alias used throughout network sessions.
/// </summary>
public readonly record struct SessionId(uint Value)
{
	private static uint _next = 0;

	public static SessionId Next() => new(Interlocked.Increment(ref _next));
	public static readonly SessionId Invalid = new(0);

	public override string ToString() => $"Session#{Value}";
}
