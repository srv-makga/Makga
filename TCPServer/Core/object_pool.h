#pragma once

#include "lock.h"
#include <queue>
#include <memory>

#define POOL(classname)			classname::ObjectPool
#define INIT_POOL(classname)	classname::ObjectPool::Initialize

namespace core {
template <typename T>
concept MemoryPoolConcept = true;// sizeof(T) >= sizeof(void*);

template<MemoryPoolConcept T>
class MemoryPool
{
public:
	MemoryPool() = default;
	~MemoryPool() = default;

	static void Initialize(int _init_count, int _grow_count)
	{
		m_init_count = _init_count;
		m_grow_count = _grow_count;

		Create(m_init_count);
	}

	static void* operator new(std::size_t _size)
	{
		core::LockGuard lock(m_cs);

		if (nullptr == m_free_list)
		{
			Create(m_grow_count);
		}

		if (nullptr == m_free_list)
		{
			throw std::bad_alloc();
			//return nullptr;
		}

		uint8_t* available = m_free_list;
		m_free_list = *reinterpret_cast<uint8_t**>(available);
		++m_using_count;

		return available;
	}

	static void	operator delete(void* _object)
	{
		core::LockGuard lock(m_cs);

		if (0 == m_using_count)
		{
			return;
		}

		--m_using_count;

		*reinterpret_cast<uint8_t**>(_object) = m_free_list;
		m_free_list = static_cast<uint8_t*>(_object);
	}

private:
	static bool Create(int _count)
	{
		if (nullptr != m_free_list)
		{
			return false;
		}

		if (0 >= _count)
		{
			return false;
		}

		m_free_list = new uint8_t[sizeof(T) * _count];

		uint8_t* next = m_free_list;
		uint8_t** curr = reinterpret_cast<uint8_t**>(m_free_list);

		for (int i = 0; i < _count - 1; ++i)
		{
			next += sizeof(T);
			*curr = next;
			curr = reinterpret_cast<uint8_t**>(next);
		}

		*curr = 0; // 마지막은 0으로 표시
		m_total_alloc_count += _count;

		return true;
	}

	static void Destroy()
	{
		core::LockGuard lock(m_cs);

		delete m_free_list;
	}

private:
	static CriticalSection m_cs;

	static uint8_t* m_free_list;
	static int m_init_count;
	static int m_grow_count;
	static int m_total_alloc_count;
	static int m_using_count;
};

template <MemoryPoolConcept T>
CriticalSection MemoryPool<T>::m_cs;

template <MemoryPoolConcept T>
uint8_t* MemoryPool<T>::m_free_list = nullptr;

template <MemoryPoolConcept T>
int MemoryPool<T>::m_init_count = 0;

template <MemoryPoolConcept T>
int MemoryPool<T>::m_grow_count = 0;

template <MemoryPoolConcept T>
int MemoryPool<T>::m_total_alloc_count = 0;

template <MemoryPoolConcept T>
int MemoryPool<T>::m_using_count = 0;


template<typename T>
concept ObjectPoolConcept = std::is_pointer_v<T>/* ||
	(std::is_same_v<T, std::shared_ptr<typename T::element_type>> && std::is_object_v<typename T::element_type>) ||
	(std::is_same_v<T, std::unique_ptr<typename T::element_type>> && std::is_object_v<typename T::element_type>)*/;

// @todo 포인터 타입이 아닌 타입 자체를 받고 내부에서 포인터로 관리하자
template<ObjectPoolConcept T>
class ObjectPool : protected std::queue<T>
{
public:
	using Queue_t = std::queue<T>;

public:
	ObjectPool() = default;
	~ObjectPool() = default;

	template<typename... ARGS>
	static void Initialize(std::size_t _max_size, std::size_t _extend_size, ARGS&&... _args)
	{
		m_max_size = _max_size;
		m_extend_size = _extend_size;
		Create(_max_size, std::forward<ARGS>(_args));
	}

	static void Finalize()
	{
		Destroy();
	}

	static T Pop()
	{
		core::WriteLock lock(m_mutex);

		if (true == m_queue.empty())
		{
			if (0 >= m_extend_size)
			{
				return nullptr;
			}

			Create(m_extend_size);
		}

		if (true == m_queue.empty())
		{
			return nullptr;
		}

		auto obj = m_queue.front();
		m_queue.pop();

		return obj;
	}


	static bool Push(T obj)
	{
		if (nullptr == obj)
		{
			return false;
		}

		obj->Initialize();
		{
			core::WriteLock lock(m_mutex);
			m_queue.push(obj);
		}

		return true;
	}

	static std::size_t Size()
	{
		core::WriteLock lock(m_mutex);
		return m_queue.size();
	}

	//static std::size_t MaxSize() { core::ReadLock lock(m_mutex); return m_max_size; }
	//static std::size_t FreeSize() { core::ReadLock lock(m_mutex); return m_queue.size(); }

protected:
	template<typename... ARGS>
	static bool Create(std::size_t _size, ARGS&&... _args)
	{
		m_max_size += _size;

		for (int i = 0; i < _size; ++i)
		{
			//if constexpr (std::is_same_v<T, std::unique_ptr<typename T::element_type>>)
			//{
			//	m_queue.push(std::make_unique<T>());
			//}
			//else if constexpr (std::is_same_v<T, std::shared_ptr<typename T::element_type>>)
			//{
			//	m_queue.push(std::make_shared<T>());
			//}
			//else
			{
				m_queue.push(new std::remove_pointer_t<T>(std::forward<ARGS>(_args)...));
			}
		}

		return true;
	}

	static void Destroy()
	{
		core::WriteLock lock(m_mutex);

		while (false == m_queue.empty())
		{
			if constexpr (std::is_pointer_v<T>)
			{
				T object = m_queue.front();
				m_queue.pop();
				::operator delete(object);
			}
			else
			{
				m_queue.pop();
			}
		}
	}

protected:
	inline static std::size_t m_max_size = 0;
	inline static std::size_t m_extend_size = 0;
	inline static Queue_t m_queue;
	inline static core::RWMutex m_mutex;
};
} // namespace core
