#pragma once

#include "../Core/lock.h"
#include "../Core/singleton.hpp"
#include <thread>

// @brief 서로 다른 작업을 처리하는 스레드 클래스
// @detail 작업을 푸시한다
class ThreadManager : public core::pattern::Singleton<ThreadManager>
{
public:
	using Thread = std::thread;
	using Threads = std::vector<Thread>;

	ThreadManager();
	virtual ~ThreadManager();

	void Initialize();
	void Finalize();

	void CreateThread(std::function<void()> _work);
	void Stop();

private:
	core::RWMutex m_mutex_thread;
	Threads m_threads;
};