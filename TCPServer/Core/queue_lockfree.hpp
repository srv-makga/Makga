#pragma once
#include "core_header.h"
#include <memory>

namespace core {
namespace queue {
template<typename T>
class QueueLockFree final
{
private:
	struct Node;

	struct TaggedPointer
	{
		Node* ptr;
		uintptr_t tag;

		TaggedPointer(Node* p = nullptr, uintptr_t t = 0) : ptr(p), tag(t) {}

		bool operator==(const TaggedPointer& other) const
		{
			return ptr == other.ptr && tag == other.tag;
		}
	};

	struct Node
	{
		std::shared_ptr<T> data;
		std::atomic<TaggedPointer> next;

		Node() : next(TaggedPointer(nullptr, 0)) {}
		Node(T value) : data(std::make_shared<T>(value)), next(TaggedPointer(nullptr, 0)) {}
	};

	std::atomic<TaggedPointer> head;
	std::atomic<TaggedPointer> tail;

public:
	QueueLockFree()
	{
		Node* dummy = new Node();
		head.store(TaggedPointer(dummy, 0));
		tail.store(TaggedPointer(dummy, 0));
	}

	~QueueLockFree()
	{
		while (Node* node = head.load().ptr)
		{
			head.store(TaggedPointer(node->next.load().ptr, 0));
			delete node;
		}
	}

	void Enqueue(T value)
	{
		Node* new_node = new Node(value);
		TaggedPointer old_tail;
		while (true)
		{
			old_tail = tail.load();
			TaggedPointer next = old_tail.ptr->next.load();
			if (old_tail == tail.load())
			{
				if (next.ptr == nullptr)
				{
					// 다음 노드가 없다면, 여기서 새로운 노드를 연결 시도
					TaggedPointer new_next(new_node, next.tag + 1);
					if (old_tail.ptr->next.compare_exchange_weak(next, new_next))
					{
						// tail을 새로운 노드로 이동
						tail.compare_exchange_weak(old_tail, TaggedPointer(new_node, old_tail.tag + 1));
						return;
					}
				}
				else
				{
					// tail이 뒤쳐진 경우 tail을 한 칸 앞으로 이동
					tail.compare_exchange_weak(old_tail, TaggedPointer(next.ptr, old_tail.tag + 1));
				}
			}
		}
	}

	std::shared_ptr<T> Dequeue()
	{
		TaggedPointer old_head;
		while (true)
		{
			old_head = head.load();
			TaggedPointer old_tail = tail.load();
			TaggedPointer next = old_head.ptr->next.load();

			if (old_head == head.load())
			{
				if (old_head.ptr == old_tail.ptr)
				{
					// 큐가 비었을 가능성 있음
					if (next.ptr == nullptr)
					{
						return std::shared_ptr<T>();  // 빈 큐
					}
					// tail이 늦게 따라오고 있으므로 tail을 다음 노드로 이동
					tail.compare_exchange_weak(old_tail, TaggedPointer(next.ptr, old_tail.tag + 1));
				}
				else
				{
					// head를 한 칸 앞으로 이동하고 데이터를 반환
					if (head.compare_exchange_weak(old_head, TaggedPointer(next.ptr, old_head.tag + 1)))
					{
						std::shared_ptr<T> result = next.ptr->data;
						delete old_head.ptr;
						return result;
					}
				}
			}
		}
	}
};
} // namespace queue
} // namespace core