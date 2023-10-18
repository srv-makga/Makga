#pragma once

#include "../Core/thread_group.h"
#include <chrono>
#include <functional>
#include <map>
#include <set>

class event_object
{
public:
	using func_t = std::function<void(void)>;
	using time_t = std::time_t;

private:
	func_t m_func;
	time_t m_time;

public:
	event_object(func_t&& _func, time_t _time)
		: m_func(std::move(_func))
		, m_time(_time)
	{}

	const func_t& func() const
	{
		return m_func;
	}

	time_t time() const
	{
		return m_time;
	}

	bool operator<(const event_object& _other) const
	{
		return m_time < _other.time();
	}
};



class event_timer
{
	std::multiset<event_object> m_list;
	core::thread::ThreadGroup m_thread_group;

public:
	event_timer() = default;

	void Initialize();

	bool add(const event_object& _event);
	bool add(event_object&& _event);

	void Run();
};
