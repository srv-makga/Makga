#pragma once

#include "queue_interface.h"
#include "lock.h"
#include <queue>
#include <condition_variable>

namespace core {
namespace container {
// @breif 알림 기능이 추가된 큐
// @detail 요소의 추가,삭제시 스레드 세이프하며, Push 발생시 Pop에서 대기하고 있는 스레드에 알림
template<typename T>
class QueueSafeCv : public QueueInterface
{
	using Container_t = std::queue<T>;

public:
	QueueSafeCv() = default;
	~QueueSafeCv()
	{
		Finalize();
	}

	bool Initialize() override
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
		m_cv.notify_one(); // pop에서 대기하고 있는 스레드 하나에 알림
	}

	void Push(T&& _item)
	{
		std::unique_lock lock(m_mutex);

		m_queue.push(_item);
		lock.unlock();
		m_cv.notify_one(); // pop에서 대기하고 있는 스레드 하나에 알림
	}

	T Pop()
	{
		std::unique_lock lock(m_mutex);

		m_cv.wait<>(lock, [this]() { return !m_queue.empty(); });

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
	std::condition_variable m_cv;
};
} // namespace container
} // namespace core