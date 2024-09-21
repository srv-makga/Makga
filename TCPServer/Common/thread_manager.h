#pragma once

#include "thread_base.h"
#include "../Core/singleton.hpp"

// @brief ���� �ٸ� �۾��� ó���ϴ� ������ Ŭ����
// @detail �۾��� Ǫ���Ѵ�
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