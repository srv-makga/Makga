#pragma once

#include "lock.h"
#include <vector>

namespace core {
namespace container {
template<typename T>
class VectorSafe
{
	using Container_t = std::vector<T>;

public:
	VectorSafe() = default;
	~VectorSafe()
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

		for (auto& iter : m_container)
		{
			if constexpr (std::is_pointer_v<T>)
			{
				delete iter;
			}
		}

		m_container.clear();
	}

	void Resize(std::size_t _size)
	{
		std::unique_lock lock(m_mutex);
		m_container.resize(_size);
	}

	void Reserve(std::size_t _size)
	{
		std::unique_lock lock(m_mutex);
		m_container.reserve(_size);
	}

	void PushBack(const T& _item)
	{
		std::unique_lock lock(m_mutex);
		m_container.push_back(_item);
	}

	T& operator[](size_t _pos)
	{
		std::unique_lock lock(m_mutex);
		return m_container.operator[_pos];
	}

	void PushBack(T&& _item)
	{
		/*m_container.insert();
		m_container.pop_back();
		m_container.swap();
		m_container.assign();*/

		std::unique_lock lock(m_mutex);
		m_container.push_back(_item);
	}

	Container_t::iterator Erase(Container_t::iterator _iter)
	{
		std::unique_lock lock(m_mutex);
		return m_container.erase(_iter);
	}

	Container_t::iterator Begin()
	{
		std::unique_lock lock(m_mutex);
		return m_container.begin();
	}

	Container_t::iterator End()
	{
		std::unique_lock lock(m_mutex);
		return m_container.end();
	}

	bool IsEmpty() const
	{
		std::unique_lock lock(m_mutex);
		return m_container.empty();
	}

	std::size_t Size() const
	{
		std::unique_lock lock(m_mutex);
		return m_container.size();
	}

private:
	Container_t m_container;
	mutable RCMutex m_mutex;
};
} // namespace container
} // namespace core