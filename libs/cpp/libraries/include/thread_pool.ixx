module;

export module makga.lib.thread.pool;

import <thread>;
import <functional>;
import <vector>;
import makga.lib.doublequeue;

export namespace makga::lib {
// @brief Each thread processes the queued task
export class ThreadPool
{
	using Job_t = std::function<void(void)>;

public:
	ThreadPool() = default;
	virtual ~ThreadPool() = default;

	bool Initialize(std::size_t thread_count);
	void Finalize();

	void Push(Job_t&& job);
	Job_t Pop();

	void Stop();

	std::size_t ThreadCount() const { return threads_.size(); }

private:
	std::vector<std::jthread> threads_;
	makga::lib::DoubleQueue<Job_t> job_queue_;
	std::condition_variable cv_;
	std::mutex mutex_;
};
} // namespace makga::lib