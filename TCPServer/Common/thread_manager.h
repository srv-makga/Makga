#pragma once

#include "thread_base.h"
#include "../Core/singleton.hpp"

class ThreadManager : public JobHandler, public core::pattern::Singleton<ThreadManager>
{
public:
	using Thread = ThreadBase;
	using Threads = std::vector<Thread*>;

	ThreadManager();
	virtual ~ThreadManager();

	void Initialize();
	void Finalize();

	void InitThread(std::size_t _thread_count);

	void Push(Job* _job) override;

private:
	Threads m_threads;
};

#define JOB_HANDLER		ThreadManager::Instance()