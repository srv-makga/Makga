#pragma once

#include "pool.h"
#include "../Common/job_handler.h"
#include "../Core/thread_pool.hpp"

class WorkerThread : public JobHandler, public core::thread::ThreadPool<std::shared_ptr<Job>>
{
public:
	WorkerThread(ThreadId_t _id);
	virtual ~WorkerThread();

public: // JobHandler
	bool Initialize() override;
	void Finalize() override;

	bool CreateThread(std::size_t _thread_count = 1) override;
	void Push(TaskType _job) override;

protected:
	const ThreadId_t m_id;
};