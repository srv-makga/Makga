#pragma once

#include "common_header.h"

class Job
{
public:
	Job(std::function<void()> callback) :m_callback(callback) {}

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
		m_callback();
	}

	std::size_t ThreadId() const
	{
		return m_thread_id;
	}

private:
	std::function<void()> m_callback;
	std::size_t m_thread_id = 0;
};