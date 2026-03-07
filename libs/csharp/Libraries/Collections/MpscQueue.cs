using System.Threading.Channels;

namespace Makga.Libraries.Collections;

/// <summary>
/// Multi-Producer Single-Consumer queue built on System.Threading.Channels.
/// Equivalent to C++ MPSCQueue&lt;T&gt;.
/// All TryEnqueue calls are thread-safe; TryDequeue/DequeueAsync must be
/// called from a single consumer thread only.
/// </summary>
public sealed class MpscQueue<T>
{
	private readonly Channel<T> _channel;

	public MpscQueue(int? boundedCapacity = null)
	{
		_channel = null == boundedCapacity
			? Channel.CreateUnbounded<T>(new UnboundedChannelOptions { SingleReader = true })
			: Channel.CreateBounded<T>(new BoundedChannelOptions(boundedCapacity.Value) { SingleReader = true });
	}

	/// <summary>Enqueue from any producer thread. Non-blocking.</summary>
	public bool TryEnqueue(T item) => _channel.Writer.TryWrite(item);

	/// <summary>Non-blocking dequeue for the single consumer.</summary>
	public bool TryDequeue(out T? item) => _channel.Reader.TryRead(out item);

	/// <summary>Async dequeue awaited by the single consumer.</summary>
	public ValueTask<T> DequeueAsync(CancellationToken ct = default) =>
		_channel.Reader.ReadAsync(ct);

	/// <summary>Async enumeration of all items as they arrive.</summary>
	public IAsyncEnumerable<T> ReadAllAsync(CancellationToken ct = default) =>
		_channel.Reader.ReadAllAsync(ct);

	/// <summary>Signal that no more items will be produced.</summary>
	public void Complete() => _channel.Writer.Complete();
}
