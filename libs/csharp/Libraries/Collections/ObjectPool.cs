using System.Collections.Concurrent;

namespace Makga.Libraries.Collections;

/// <summary>
/// Generic object pool. Acquired objects are returned to the pool on dispose
/// instead of being garbage collected.
/// Equivalent to C++ ObjectPool&lt;T&gt; with CustomDeleter.
/// </summary>
public sealed class ObjectPool<T> where T : class
{
	private readonly ConcurrentBag<T> _pool = new();
	private readonly Func<T> _factory;
	private readonly Action<T>? _reset;

	/// <param name="factory">Creates a new instance when the pool is empty.</param>
	/// <param name="reset">Optional: resets an object's state before returning it to the pool.</param>
	public ObjectPool(Func<T> factory, Action<T>? reset = null)
	{
		_factory = factory;
		_reset = reset;
	}

	/// <summary>Pre-populate the pool with <paramref name="count"/> instances.</summary>
	public void Prewarm(int count)
	{
		for (var i = 0; i < count; i++)
		{
			_pool.Add(_factory());
		}
	}

	/// <summary>
	/// Acquire an object from the pool (or create a new one if empty).
	/// Dispose the returned <see cref="PooledObject{T}"/> to return it to the pool.
	/// </summary>
	public PooledObject<T> Acquire()
	{
		var obj = _pool.TryTake(out var pooled) ? pooled : _factory();
		return new PooledObject<T>(obj, this);
	}

	internal void Return(T obj)
	{
		_reset?.Invoke(obj);
		_pool.Add(obj);
	}
}

/// <summary>RAII handle — returns the wrapped object to its pool on Dispose.</summary>
public readonly struct PooledObject<T> : IDisposable where T : class
{
	public T Value { get; }
	private readonly ObjectPool<T> _pool;

	internal PooledObject(T value, ObjectPool<T> pool)
	{
		Value = value;
		_pool = pool;
	}

	public void Dispose() => _pool.Return(Value);
}
