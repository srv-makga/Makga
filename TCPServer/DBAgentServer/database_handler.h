#pragma once

#include "database_thread.h"
#include "../Common/thread_base.h"
#include "../Core/singleton.hpp"

class DatabaseClient;

class DatabaseHandler : public JobHandler, public core::pattern::Singleton<DatabaseHandler>
{
public:
	using Thread = DatabaseThread;
	using Threads = std::vector<Thread*>;

	DatabaseHandler();
	virtual ~DatabaseHandler();

	void Initialize();
	void Finalize();

	void InitThread(std::size_t _thread_count);

	void Push(Job* _job) override;

private:
	Threads m_threads;
};