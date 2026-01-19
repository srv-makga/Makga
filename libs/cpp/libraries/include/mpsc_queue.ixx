module;

export module makga.lib.mpsc_queue;

import <queue>;
import <atomic>;
import makga.lib.thread_local_pool;

export namespace makga::lib {
export template<typename T>
class MPSCQueue
{
public:
	struct Node
	{
		T data;
		Node* next;
	};

	using Pool = makga::lib::ThreadLocalPool<Node>;

public:
	MPSCQueue()
	{
		Node* dummy = Pool::Allocate();
		dummy->next = nullptr;

		head_ = dummy;
		tail_.store(dummy, std::memory_order_relaxed);
	}

	virtual ~MPSCQueue()
	{
		T temp;
		while (Dequeue(temp))
		{
		}

		Pool::Deallocate(head_);
	}

	void Enquque(const T& value)
	{
		Node* new_node = Pool::Allocate();
		new_node->data = value;
		new_node->next = nullptr;

		Node* old_tail = tail_.exchange(new_node, std::memory_order_acq_rel);
		old_tail->next = new_node;
	}

	bool Dequeue(T& value)
	{
		Node* next = head_->next;
		if (nullptr == next)
		{
			return false;
		}

		value = next->data;

		Pool::Deallocate(head_);

		head_ = next;

		return true;
	}

	bool IsEmpty() const
	{
		return head_ == tail_;
	}

private:
	Node* head_;
	std::atomic<Node*> tail_;
};
} // namespace makga::lib