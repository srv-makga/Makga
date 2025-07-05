#pragma once

#include "lock.h"
#include <queue>
#include <atomic>
#include <condition_variable>

namespace core {
namespace queue {
template<typename T>
class QueueSwap
{
public:
	using Queue_t = std::queue<T>;

public:
	QueueSwap()
		: m_push_queue(&m_queue1)
		, m_pop_queue(&m_queue2)
	{
	}

	bool Initialize()
	{
		return true;
	}

	void Finalize()
	{

	}

	void Push(const T& _data)
	{
		std::unique_lock lock(m_mutex);
		m_push_queue->push(_data);
	}

	void Push(T&& _data)
	{
		std::unique_lock lock(m_mutex);
		m_push_queue->push(_data);
	}

	bool Pop(T& t)
	{
		if (true == m_pop_queue->empty())
		{
			return false;
		}

		t = std::move(m_pop_queue->front());
		m_pop_queue->pop();
		return true;
	}

	// @breif T가 함수일 경우 모두 실행
	bool Flush()
	{
		if constexpr (!std::is_function_v<T>)
		{
			return false;
		}

		T* job = nullptr;

		{
			std::unique_lock lock(m_mutex);

			if (true == Empty())
			{
				Swap();

				if (true == Empty())
				{
					return false;
				}
			}

			*job = std::move(m_pop_queue->front());
			m_pop_queue->pop();
		}

		job();

		return true;
	}

	bool Empty() const
	{
		std::unique_lock lock(m_mutex);
		return m_pop_queue->empty();
	}

	std::size_t Size() const
	{
		std::unique_lock lock(m_mutex);
		return m_queue1.size() + m_queue2.size();
	}

	void Swap()
	{
		std::unique_lock lock(m_mutex);
		std::swap(m_push_queue, m_pop_queue);
	}

private:
	Queue_t m_queue1;
	Queue_t m_queue2;
	Queue_t* m_push_queue;
	Queue_t* m_pop_queue;

	std::condition_variable_any m_cv;

	mutable RCMutex m_mutex;
};
} // namespace queue
} // namespace core