#pragma once

#include "database_client.h"
#include "job_handler.h"
#include "../Core/thread.h"
#include "../Core/queue_safety.hpp"

class DatabaseThread : public core::thread::Thread, public JobHandler
{
public:
	using Queue_t = core::queue::QueueSafety<Job*>;

public:
	DatabaseThread() {}
	virtual ~DatabaseThread() {}

	void Run() override;
	void Push(Job* _data) override;

private:
	DatabaseClient m_client;
	Queue_t m_queue;

};