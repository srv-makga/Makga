module;

import <thread>;
import <functional>;
import <vector>;

export module makga.lib.thread.group;

export namespace makga::lib {
// @brief Multiple threads performing the same task
export class ThreadGroup
{
	using Job_t = std::function<void(void)>;

public:
	ThreadGroup() = default;
	virtual ~ThreadGroup() = default;

	bool Initialize() { return true; }
	void Finalize() { Stop(); }

	bool Start(Job_t&& job, std::size_t thread_count);
	void Stop();

	bool IsRunning() const { return running_; }
	std::size_t ThreadCount() const { return threads_.size(); }

private:
	std::vector<std::jthread> threads_;
	std::atomic<bool> running_ = false;
	Job_t job_ = nullptr;
};
} // namespace makga::lib