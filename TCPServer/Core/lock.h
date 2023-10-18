#pragma once


#include <shared_mutex>
#include <mutex>
#include <thread>
#ifdef _WIN32
#include <Windows.h>
#endif

#define SYNC(mutex)	std::unique_lock lock_##mutex(mutex);
using AutoLock = std::unique_lock<std::mutex>;


namespace core {
#ifdef _WIN32
class CriticalSection
{
public:
#if (_WIN32_WINNT >= 0x0500)
		//You can improve performance significantly by choosing a small spin count for a critical section of short duration. 
		//The heap manager uses a spin count of roughly 4000 for its per-heap critical sections. 
		//This gives great performance and scalability in almost all worst-case scenarios.
	CriticalSection(ULONG _spin_count = 4000)
	{
		m_init = ::InitializeCriticalSectionAndSpinCount(&m_cs, _spin_count);
	}
#else
	CriticalSection()
	{
		try
		{
			initializeCriticalSection(&m_stCriticalSection);
			m_init = TRUE;
		}
		catch (...)
		{
			m_init = FALSE;
		}
	}
#endif
	virtual ~CriticalSection()
	{
		if (true == m_init)
		{
			::DeleteCriticalSection(&m_cs);
		}
	}

public:
	inline void Lock() { ::EnterCriticalSection(&m_cs); }
	inline void UnLock() { ::LeaveCriticalSection(&m_cs); }

private:
	CRITICAL_SECTION m_cs;
	bool m_init;
};

class LockGuard
{
	CriticalSection& m_cs;

public:
	LockGuard() = delete;
	LockGuard(CriticalSection& _cs)
		: m_cs(_cs)
	{
		m_cs.Lock();
	}

	~LockGuard()
	{
		m_cs.UnLock();
	}
};
#endif // _WIN32

// Read & Write
using RWMutex = ::std::shared_mutex;
using ReadLock = ::std::shared_lock<RWMutex>;
using WriteLock = ::std::unique_lock<RWMutex>;

// Recursive
using RCMutex = ::std::recursive_mutex;

class RecursiveSharedMutex
{
	::std::shared_mutex m_mutex;
	::std::thread::id m_threadid;
	int m_count;

public:
	RecursiveSharedMutex()
		: m_count(0)
	{}
	virtual ~RecursiveSharedMutex()
	{}

	void Lock() noexcept
	{
		auto threadid = ::std::this_thread::get_id();
		if (threadid == m_threadid)
		{
			++m_count;
			return;
		}

		m_mutex.lock();
		m_threadid = ::std::this_thread::get_id();
		m_count = 1;
	}

	void UnLock() noexcept
	{
		if (0 < --m_count)
		{
			return;
		}

		m_threadid = ::std::thread::id();
		m_mutex.unlock();
	}

	void LockShared() noexcept
	{
		auto threadid = ::std::this_thread::get_id();
		if (threadid == m_threadid)
		{
			return;
		}

		m_mutex.lock_shared();
	}

	void UnLockShared() noexcept
	{
		auto threadid = ::std::this_thread::get_id();
		if (threadid == m_threadid)
		{
			return;
		}

		m_mutex.unlock_shared();
	}
};
} // namespace core