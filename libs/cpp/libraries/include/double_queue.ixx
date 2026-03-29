癤퓅odule;

export module makga.lib.doublequeue;

import <queue>;
import <functional>;
import <type_traits>;
import <utility>;
import makga.lib.lock;

export namespace makga::lib {
export template<typename T>
class DoubleQueue
{
public:
	using Queue_t = std::queue<T>;

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
			queue1_.pop();
		}

		while (false == queue2_.empty())
		{
			queue2_.pop();
		}
	}

	void Push(T job)
	{
		// T占쏙옙 占쏙옙占쏙옙占쏙옙占싱거놂옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占쌓댐옙占?占쏙옙占쏙옙占싹곤옙,
		// 占쏙옙 타占쏙옙占쏙옙 占쏙옙荑∽옙占?std::move占쏙옙 占싱듸옙占쏙옙킵占싹댐옙.
		std::lock_guard lock(enqueue_mutex_);
		if constexpr (std::is_pointer_v<T> || std::is_reference_v<T>)
		{
			enqueue_->push(job);
		}
		else
		{
			enqueue_->push(std::move(job));
		}
	}

	T Pop()
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
	mutable Mutex swap_mutex_;
	mutable Mutex enqueue_mutex_;

	Queue_t queue1_;
	Queue_t queue2_;
};
} // namespace makga::lib