module;

export module makga.lib.doublequeue;

import <queue>;
import <functional>;
import <mutex>;

export namespace makga::lib {
export class DoubleQueue
{
	using Job_t = std::function<void(void)>;
	using Queue_t = std::queue<Job_t>;

public:
	DoubleQueue();
	virtual ~DoubleQueue();

	void Push(Job_t&& job);
	Job_t Pop();

private:
	void SwapQueues();

private:
	Queue_t* enqueue_;
	Queue_t* dequeue_;
	mutable std::mutex swap_mutex_;
	mutable std::mutex enqueue_mutex_;

	Queue_t queue1_;
	Queue_t queue2_;
};
} // namespace makga::lib