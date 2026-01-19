module;

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

			// 여전히 tail이 꼬리 노드인지 확인
			if (tail == tail_.load(std::memory_order_acquire))
			{
				if (nullptr == next)
				{
					// tail->next를 newNode로 연결 시도
					if (tail->next.compare_exchange_weak(next, new_node, std::memory_order_release, std::memory_order_relaxed))
					{
						// 연결 성공 시 tail을 newNode로 이동 시도
						tail_.compare_exchange_weak(tail, new_node, std::memory_order_release, std::memory_order_relaxed);
						return;
					}
				}
				else
				{
					// 다른 스레드가 이미 다음 노드를 추가했으므로 tail을 다음 노드로 이동 시도
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

			// head가 여전히 유효한지 확인
			if (head == head_.load(std::memory_order_acquire))
			{
				// 큐가 비어 있거나 tail이 뒤처진 경우
				if (head == tail)
				{
					// 큐가 비어 있음
					if (nullptr == next)
					{
						return false;
					}

					// tail을 앞으로 이동 (다른 스레드가 enqueue 중)
					tail_.compare_exchange_weak(tail, next, std::memory_order_release, std::memory_order_relaxed);
				}
				else
				{
					// 실제 데이터는 next 노드에 있음
					value = next->data;

					// head를 next로 이동
					if (head_.compare_exchange_weak(head, next, std::memory_order_release, std::memory_order_relaxed))
					{
						// 이전 head(dummy 또는 사용 끝난 노드) 삭제
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