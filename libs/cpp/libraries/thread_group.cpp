#include <ranges>

module makga.lib.thread.group;

import <thread>;
import <functional>;
import <vector>;

namespace makga::lib {
bool ThreadGroup::Start(Job_t&& job, std::size_t thread_count)
{
	if (nullptr == job)
	{
		return false;
	}

	if (true == running_)
	{
		return false;
	}

	job_ = std::move(job);

	threads_.reserve(thread_count);

	for (auto i : std::views::iota(0) | std::views::take(thread_count))
	{
		threads_.push_back(std::jthread(
			[this](std::stop_token stoken)
			{
				while (false == stoken.stop_requested())
				{
					job_();
				}
			}));
	}

	running_ = true;

	return true;
}

void ThreadGroup::Stop()
{
	for (auto& thread : threads_)
	{
		thread.request_stop();
	}

	threads_.clear();

	running_ = false;
}
} // namespace makga::lib