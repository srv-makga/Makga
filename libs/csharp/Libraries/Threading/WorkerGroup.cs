namespace Makga.Libraries.Threading;

/// <summary>
/// Fixed-size group of dedicated background worker threads.
/// Equivalent to C++ ThreadGroup.
/// </summary>
public sealed class WorkerGroup : IDisposable
{
	private readonly Thread[] _threads;
	private readonly CancellationTokenSource _cts = new();
	private bool _started;

	/// <param name="count">Number of worker threads to create.</param>
	/// <param name="work">Work loop executed on each thread. Observe the
	/// <see cref="CancellationToken"/> for graceful shutdown.</param>
	/// <param name="namePrefix">Optional thread name prefix (e.g. "GameWorker").</param>
	public WorkerGroup(int count, Action<CancellationToken> work, string? namePrefix = null)
	{
		_threads = new Thread[count];
		for (var i = 0; i < count; i++)
		{
			var index = i;
			var thread = new Thread(() => work(_cts.Token))
			{
				IsBackground = true,
				Name = null != namePrefix ? $"{namePrefix}-{index}" : $"Worker-{index}"
			};
			_threads[i] = thread;
		}
	}

	public void Start()
	{
		if (_started) { return; }
		_started = true;
		foreach (var t in _threads)
		{
			t.Start();
		}
	}

	public void Stop()
	{
		_cts.Cancel();
		foreach (var t in _threads)
		{
			if (t.IsAlive) { t.Join(); }
		}
	}

	public void Dispose() => Stop();
}
