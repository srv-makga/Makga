module makga.lib.doublequeue;

import <queue>;

namespace makga::lib {
DoubleQueue::DoubleQueue()
	: enqueue_(&queue1_)
	, dequeue_(&queue2_)
{
}
DoubleQueue::~DoubleQueue()
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

void DoubleQueue::Push(Job_t&& job)
{
	std::lock_guard lock(enqueue_mutex_);
	enqueue_->push(std::move(job));
}

DoubleQueue::Job_t DoubleQueue::Pop()
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

bool DoubleQueue::IsEmpty() const
{
	return true == dequeue_->empty() && true == enqueue_->empty();
}

void DoubleQueue::SwapQueues()
{
	std::lock_guard lock(swap_mutex_);
	std::swap(enqueue_, dequeue_);
}
} // namespace makga::lib