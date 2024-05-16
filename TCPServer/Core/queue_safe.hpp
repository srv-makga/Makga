#pragma once

#include "queue_interface.h"
#include "lock.h"
#include <queue>

namespace core {
namespace container {
template<typename T>
class QueueSafe
{
	using Container_t = std::queue<T>;

public:
	QueueSafe() = default;
	~QueueSafe()
	{
		Finalize();
	}

	bool Initialize()
	{
		return true;
	}

	void Finalize()
	{
		std::unique_lock lock(m_mutex);

		while (false == m_queue.empty())
		{
			if constexpr (std::is_pointer_v<T>)
			{
				T t = m_queue.front();
				delete t;
			}

			m_queue.pop();
		}
	}

	void Push(const T& _item)
	{
		std::unique_lock lock(m_mutex);

		m_queue.push(_item);
		lock.unlock();
	}

	void Push(T&& _item)
	{
		std::unique_lock lock(m_mutex);

		m_queue.push(_item);
		lock.unlock();
	}

	T Pop()
	{
		std::unique_lock lock(m_mutex);

		T item = m_queue.front();
		m_queue.pop();

		// @todo nrvo 가 되는지 확인 필요
		return item;
	}

	T TryPop()
	{
		T ret;

		{
			std::unique_lock lock(m_mutex);

			if (false == m_queue.empty())
			{
				ret = m_queue.front();
				m_queue.pop();
			}
		}

		return ret;
	}

	bool IsEmpty() const
	{
		std::unique_lock lock(m_mutex);
		return m_queue.empty();
	}

private:
	Container_t m_queue;
	mutable std::mutex m_mutex;
};
} // namespace container
} // namespace core