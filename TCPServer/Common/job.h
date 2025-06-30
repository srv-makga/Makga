#pragma once

#include "common_header.h"

class Job
{
public:
	Job() = default;
	Job(std::function<void()> callback, std::size_t _thread_id)
		: m_callback(callback)
		, m_thread_id(_thread_id)
	{
	}

	template<typename T, typename Ret, typename...Args>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args... args)
	{
		m_callback = [owner, memFunc, args...]()
			{
				(owner.get()->*memFunc)(args...);
			};

		m_thread_id = owner->ThreadId();
	}

	void Execute()
	{
		if (nullptr != m_callback)
		{
			m_callback();
		}
	}

	std::size_t ThreadId() const
	{
		return m_thread_id;
	}

private:
	std::function<void()> m_callback;
	std::size_t m_thread_id = 0;
};