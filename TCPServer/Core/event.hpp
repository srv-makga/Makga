#pragma once

#include "core_header.h"

template <typename... Args>
class Event
{
	std::vector<std::function<void(Args...)>> m_subscribers;

public:
	Event() = default;

	void operator+=(std::function<void(Args&&...)>&& f)
	{
		m_subscribers.push_back(f);
	}

	void operator-=(std::function<void(Args&&...)>&& f)
	{
		m_subscribers.erase(
			std::remove(m_subscribers.begin(), m_subscribers.end(), f),
			m_subscribers
		);
	}

	void operator()(Args...&& args)
	{
		for (auto& f : m_subscribers)
		{
			f(args...);
		}
	}
};