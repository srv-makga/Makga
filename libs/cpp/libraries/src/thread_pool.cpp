#include <ranges>

module makga.lib.thread.pool;

import <thread>;
import <functional>;
import <vector>;

namespace makga::lib {
bool ThreadPool::Initialize(std::size_t thread_count)
{
	if (0 == thread_count)
	{
		return false;
	}

	threads_.reserve(thread_count);

	for (std::size_t i = 0; i < thread_count; ++i)
	{
		threads_.push_back(std::jthread(
			[this](std::stop_token stoken)
			{
				Job_t job = nullptr;

				while (false == stoken.stop_requested())
				{
					{
						std::unique_lock<std::mutex> lock(this->mutex_);

						this->cv_.wait(lock, [this] { return false == this->job_queue_.IsEmpty(); });

						job = std::move(this->job_queue_.Pop());
					}

					if (nullptr != job)
					{
						job();
					}
				}
			}
		));
	}

	return true;
}

void ThreadPool::Push(Job_t&& job)
{
	job_queue_.Push(std::move(job));
}

ThreadPool::Job_t ThreadPool::Pop()
{
	return job_queue_.Pop();
}
} // namespace makga::lib