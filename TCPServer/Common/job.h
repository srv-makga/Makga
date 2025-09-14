#pragma once

#include "common_header.h"
#include "../Core/object_pool.h"

class Job : public std::enable_shared_from_this<Job>
{
public:
	Job()
		: m_callback(nullptr)
		, m_thread_id(0)
	{
	}

	bool Initialize()
	{
		m_callback = nullptr;
		m_thread_id = 0;
		return true;
	}

	void Finalize()
	{
		m_callback = nullptr;
		m_thread_id = 0;
	}

	bool Set(std::function<void()> callback, std::size_t _thread_id)
	{
		if (nullptr == callback)
		{
			return false;
		}

		m_callback = callback;
		m_thread_id = _thread_id;
		return true;
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

public:
	inline static std::shared_ptr<Job> Pop()
	{
		return s_pool.Pop();
	}

	inline static void InitPool(std::size_t _initial_size, std::function<Job*()> _create_func)
	{
		s_pool.Initialize(_initial_size, _create_func);
	}

	inline static void ClearPool()
	{
		s_pool.Finalize();
	}

	inline static core::SharedObjectPool<Job> s_pool;

private:
	std::function<void()> m_callback;
	std::size_t m_thread_id = 0;
};