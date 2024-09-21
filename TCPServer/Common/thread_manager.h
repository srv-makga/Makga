#pragma once

#include "thread_base.h"
#include "../Core/singleton.hpp"

// @brief 서로 다른 작업을 처리하는 스레드 클래스
// @detail 작업을 푸시한다
class ThreadManager : public JobHandler, public core::pattern::Singleton<ThreadManager>
{
public:
	using Thread = std::shared_ptr<ThreadBase>;
	using Threads = std::vector<Thread>;

	ThreadManager();
	virtual ~ThreadManager();

	void Initialize();
	void Finalize();

	bool CreateThread(std::size_t _thread_count);
	void Stop();
	void Push(Job_t _job) override;

private:
	Threads m_threads;
};