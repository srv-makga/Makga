#pragma once

#include "../Core/thread.h"
#include "../Core/queue_safety.hpp"
#include "job_handler.h"

class ThreadBase : public core::thread::Thread, public JobHandler
{
public:
	using Queue_t = core::queue::QueueSafety<Job*>;
	
	ThreadBase();
	virtual ~ThreadBase();

	void Initialize();
	void Finalize();

	void Push(Job* _job) override;

protected:
	void Run() override;

protected:
	Queue_t m_queue;
};