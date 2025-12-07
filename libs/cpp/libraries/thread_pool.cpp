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
	for (auto i : std::views::iota(0) | std::views::take(thread_count))
	{
		threads_.push_back(std::jthread(
			[this](std::stop_token stoken)
			{
				// @todo 큐잉된 작업 처리
				while (false == stoken.stop_requested())
				{
				}
			}));
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