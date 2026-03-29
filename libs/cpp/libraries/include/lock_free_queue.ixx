癤퓅odule;

export module makga.lib.lock_free_queue;

import <queue>;
import <atomic>;

export namespace makga::lib {
export template<typename T>
class LockFreeQueue
{
public:
	struct Node
	{
		Node() : next(nullptr) {}
		Node(const T& value) : data(value), next(nullptr) {}

		T data;
		Node* next;
	};

public:
	LockFreeQueue()
	{
		Node* dummy = new Node;
		head_.store(dummy, std::memory_order_relaxed);
		tail_.store(dummy, std::memory_order_relaxed);
	}

	virtual ~LockFreeQueue()
	{
		while (Node* node = head_.load())
		{
			head_.store(node->next);
			delete node;
		}
	}

	void Enquque(const T& value)
	{
		Node* new_node = new Node(value);

		while (true)
		{
			Node* tail = tail_.load(std::memory_order_acquire);
			Node* next = tail->next.load(std::memory_order_acquire);

			// 占쏙옙占쏙옙占쏙옙 tail占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙占?확占쏙옙
			if (tail == tail_.load(std::memory_order_acquire))
			{
				if (nullptr == next)
				{
					// tail->next占쏙옙 newNode占쏙옙 占쏙옙占쏙옙 占시듸옙
					if (tail->next.compare_exchange_weak(next, new_node, std::memory_order_release, std::memory_order_relaxed))
					{
						// 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙 tail占쏙옙 newNode占쏙옙 占싱듸옙 占시듸옙
						tail_.compare_exchange_weak(tail, new_node, std::memory_order_release, std::memory_order_relaxed);
						return;
					}
				}
				else
				{
					// 占쌕몌옙 占쏙옙占쏙옙占썲가 占싱뱄옙 占쏙옙占쏙옙 占쏙옙躍?占쌩곤옙占쏙옙占쏙옙占실뤄옙 tail占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占싱듸옙 占시듸옙
					tail_.compare_exchange_weak(tail, next, std::memory_order_release, std::memory_order_relaxed);
				}
			}
		}
	}

	bool Dequeue(T& value)
	{
		while (true)
		{
			Node* head = head_.load(std::memory_order_acquire);
			Node* tail = tail_.load(std::memory_order_acquire);
			Node* next = head->next.load(std::memory_order_acquire);

			// head占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙효占쏙옙占쏙옙 확占쏙옙
			if (head == head_.load(std::memory_order_acquire))
			{
				// 큐占쏙옙 占쏙옙占?占쌍거놂옙 tail占쏙옙 占쏙옙처占쏙옙 占쏙옙占?
				if (head == tail)
				{
					// 큐占쏙옙 占쏙옙占?占쏙옙占쏙옙
					if (nullptr == next)
					{
						return false;
					}

					// tail占쏙옙 占쏙옙占쏙옙占쏙옙 占싱듸옙 (占쌕몌옙 占쏙옙占쏙옙占썲가 enqueue 占쏙옙)
					tail_.compare_exchange_weak(tail, next, std::memory_order_release, std::memory_order_relaxed);
				}
				else
				{
					// 占쏙옙占쏙옙 占쏙옙占쏙옙占싶댐옙 next 占쏙옙恙?占쏙옙占쏙옙
					value = next->data;

					// head占쏙옙 next占쏙옙 占싱듸옙
					if (head_.compare_exchange_weak(head, next, std::memory_order_release, std::memory_order_relaxed))
					{
						// 占쏙옙占쏙옙 head(dummy 占실댐옙 占쏙옙占?占쏙옙占쏙옙 占쏙옙占? 占쏙옙占쏙옙
						delete head;
						return true;
					}
				}
			}
		}
	}

	bool IsEmpty() const
	{
		return head_ == tail_;
	}

private:
	std::atomic<Node*> head_;
	std::atomic<Node*> tail_;
};
} // namespace makga::lib