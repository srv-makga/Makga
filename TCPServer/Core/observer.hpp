#pragma once

#include "lock.h"
#include <set>
#include <functional>

namespace core {
namespace pattern {
template <typename T>
concept Observer_t = std::is_pointer_v<T>;

/*
@brief 옵저버 패턴
@detail thread-safe
*/

template<Observer_t T>
class Observer
{
	mutable RWMutex m_mutex;
	std::set<T> m_list;

public:
	void Attach(T object)
	{
		WriteLock lock(m_mutex);

		m_list.insert(object);
	}

	void Detach(T object)
	{
		WriteLock lock(m_mutex);

		m_list.erase(object);
	}

	void Notify(std::function<void(T)>&& _action) const
	{
		ReadLock lock(m_mutex);

		if (true == m_list.empty())
		{
			return;
		}

		for (T obj : m_list)
		{
			_action(obj);
		}
	}
};
} // namespace pattern
} // namespace core