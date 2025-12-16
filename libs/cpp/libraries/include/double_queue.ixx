module;

#include <memory>

export module makga.lib.doublequeue;

import <queue>;
import <functional>;

export namespace makga::lib {
export template<typename T>
class DoubleQueue
{
public:
	using Job_t = std::shared_ptr<T>;
	using Queue_t = std::queue<Job_t>;

public:
	DoubleQueue()
		: enqueue_(&queue1_)
		, dequeue_(&queue2_)
	{
	}

	virtual ~DoubleQueue()
	{
		while (false == queue1_.empty())
		{
			auto job = queue1_.front();
			job();
			queue1_.pop();
		}

		while (false == queue2_.empty())
		{
			auto job = queue2_.front();
			job();
			queue2_.pop();
		}
	}

	void Push(Job_t job)
	{
		std::lock_guard lock(enqueue_mutex_);
		enqueue_->push(std::move(job));
	}

	Job_t Pop()
	{
		if (true == dequeue_->empty())
		{
			SwapQueues();

			if (true == dequeue_->empty())
			{
				return nullptr;
			}
		}

		auto job = dequeue_->front();
		dequeue_->pop();
		return job;
	}

	bool IsEmpty() const
	{
		return true == dequeue_->empty() && true == enqueue_->empty();
	}

private:
	void SwapQueues()
	{
		std::lock_guard lock(swap_mutex_);
		std::swap(enqueue_, dequeue_);
	}

private:
	Queue_t* enqueue_;
	Queue_t* dequeue_;
	mutable std::mutex swap_mutex_;
	mutable std::mutex enqueue_mutex_;

	Queue_t queue1_;
	Queue_t queue2_;
};
} // namespace makga::lib