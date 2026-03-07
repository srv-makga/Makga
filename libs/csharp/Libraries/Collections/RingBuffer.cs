using System.Buffers;

namespace Makga.Libraries.Collections;

/// <summary>
/// Thread-safe circular byte buffer backed by ArrayPool.
/// Eliminates compaction overhead of linear buffers.
/// Equivalent to C++ RingBuffer&lt;T&gt;.
/// </summary>
public sealed class RingBuffer : IDisposable
{
	private readonly byte[] _buffer;
	private readonly int _capacity;
	private int _readPos;
	private int _writePos;
	private int _count;
	private readonly object _lock = new();

	public RingBuffer(int capacity)
	{
		_capacity = capacity;
		_buffer = ArrayPool<byte>.Shared.Rent(capacity);
	}

	/// <summary>Bytes available to read.</summary>
	public int Available
	{
		get
		{
			lock (_lock)
			{
				return _count;
			}
		}
	}

	/// <summary>Free bytes available for writing.</summary>
	public int Free
	{
		get
		{
			lock (_lock)
			{
				return _capacity - _count;
			}
		}
	}

	/// <returns>true if all bytes were written; false if insufficient space.</returns>
	public bool TryWrite(ReadOnlySpan<byte> data)
	{
		lock (_lock)
		{
			if (data.Length > _capacity - _count)
			{
				return false;
			}
			WriteInternal(data);
			return true;
		}
	}

	/// <returns>Number of bytes actually read and consumed.</returns>
	public int Read(Span<byte> dest)
	{
		lock (_lock)
		{
			var toRead = Math.Min(dest.Length, _count);
			ReadInternal(dest[..toRead]);
			return toRead;
		}
	}

	/// <summary>Copy bytes without advancing the read position.</summary>
	public int Peek(Span<byte> dest)
	{
		lock (_lock)
		{
			var toPeek = Math.Min(dest.Length, _count);
			PeekInternal(dest[..toPeek]);
			return toPeek;
		}
	}

	/// <summary>Advance the read position without copying data.</summary>
	public void Skip(int count)
	{
		lock (_lock)
		{
			var toSkip = Math.Min(count, _count);
			_readPos = (_readPos + toSkip) % _capacity;
			_count -= toSkip;
		}
	}

	private void WriteInternal(ReadOnlySpan<byte> data)
	{
		var firstChunk = Math.Min(data.Length, _capacity - _writePos);
		data[..firstChunk].CopyTo(_buffer.AsSpan(_writePos));
		if (data.Length > firstChunk)
		{
			data[firstChunk..].CopyTo(_buffer.AsSpan(0));
		}
		_writePos = (_writePos + data.Length) % _capacity;
		_count += data.Length;
	}

	private void ReadInternal(Span<byte> dest)
	{
		PeekInternal(dest);
		_readPos = (_readPos + dest.Length) % _capacity;
		_count -= dest.Length;
	}

	private void PeekInternal(Span<byte> dest)
	{
		var firstChunk = Math.Min(dest.Length, _capacity - _readPos);
		_buffer.AsSpan(_readPos, firstChunk).CopyTo(dest);
		if (dest.Length > firstChunk)
		{
			_buffer.AsSpan(0, dest.Length - firstChunk).CopyTo(dest[firstChunk..]);
		}
	}

	public void Dispose()
	{
		ArrayPool<byte>.Shared.Return(_buffer);
	}
}
