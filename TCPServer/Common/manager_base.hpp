#pragma once

#include "../Core/lock.h"
#include <type_traits>
#include <concurrent_unordered_map.h>

template <typename T>
concept ManagerElement = std::is_pointer_v<T>;

template<typename Identity_t, ManagerElement Object_t>
class ManagerBase
{
public:
	ManagerBase() = default;
	virtual ~ManagerBase()
	{
		for (auto& iter : m_objects)
		{
			delete iter.second;
		}
	}

	virtual bool Add(Identity_t _identity, Object_t _object)
	{
		core::WriteLock lock(m_mutex);
		return m_objects.insert({ _identity, _object }).second;
	}

	virtual bool Remove(Identity_t _identity)
	{
		core::WriteLock lock(m_mutex);
		return 0 < m_objects.erase(_identity);
	}

	virtual const Object_t Find(Identity_t _identity) const
	{
		core::ReadLock lock(m_mutex);
		auto iter = m_objects.find(_identity);
		if (m_objects.end() == iter)
		{
			return nullptr;
		}

		return iter->second;
	}

protected:
	concurrency::concurrent_unordered_map<Identity_t, Object_t> m_objects;
	//std::unordered_map<Identity_t, Object_t> m_objects;
	mutable core::RWMutex m_mutex;
};