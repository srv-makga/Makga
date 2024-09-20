#pragma once

#include "lock.h"
#include <queue>
#include <condition_variable>
#include <memory>

template<typename T>
struct is_executable
{
	template<typename U>
	static constexpr auto check(U*) -> decltype(std::declval<U>()(), bool{})
	{
		return true;
	}

	template<typename U>
	static constexpr bool check(...)
	{
		return false;
	}

	static constexpr bool value = check<T>(nullptr);
};

namespace core {
namespace queue {
template <typename T>
class MPSCQueue
{
	struct Node
	{
		std::shared_ptr<T> data;
		Node* next;

		Node() : next(nullptr) {}
	};

public:
	MPSCQueue()
		: m_pop(&m_queue1)
		, m_push(&m_queue2)
		, m_element_count(0)
	{}

	void Push(T&& _value)
	{
		std::unique_lock lock(m_mutex);
		m_push->push(std::forward<T>(_value));
		++m_element_count;
	}

	bool Pop(T& _value)
	{
		if constexpr (false == is_executable<T>::value)
		{
			if (m_pop->empty())
			{
				swapQueue();
			}

			if (m_pop->empty())
			{
				return false;
			}

			_value = std::move(m_pop->front());
			m_pop->pop();

			--m_element_count;

			return true;
		}
		else
		{
			return false;
		}
	}

	void Flush()
	{
		if constexpr (true == is_executable<T>::value)
		{
			SwapQueue();

			while (!m_pop->empty())
			{
				m_pop->front()();
				m_pop->pop();
				--m_element_count;
			}
		}
	}

	void SwapQueue()
	{
		std::unique_lock lock(m_mutex);
		std::swap(m_pop, m_push);
	}

private:
	std::queue<T> m_queue1, m_queue2;
	std::queue<T>* m_pop;
	std::queue<T>* m_push;
	mutable std::recursive_mutex m_mutex;
	uint32_t m_element_count;
};
} // namespace queue
} // namespace core