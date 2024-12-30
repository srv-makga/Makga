#pragma once

#include "../Core/lock.h"
#include "../Core/singleton.hpp"
#include <thread>

// @brief ���� �ٸ� �۾��� ó���ϴ� ������ Ŭ����
// @detail �۾��� Ǫ���Ѵ�
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