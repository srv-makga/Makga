#pragma once

#include "worker_thread.h"
#include <vector>

class ThreadManager
{
public:
	ThreadManager();
	virtual ~ThreadManager();

	bool CreateThreads(std::function<void(void)> _work, std::size_t _count = 1);

protected:
	std::recursive_mutex m_mutex;
	ThreadId_t m_thread_id;
	std::vector<std::shared_ptr<WorkerThread>> m_threads;
};