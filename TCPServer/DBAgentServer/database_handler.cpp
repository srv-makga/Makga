#include "pch.h"
#include "database_handler.h"
#include "database_client.h"

DatabaseHandler::DatabaseHandler()
{
}

DatabaseHandler::~DatabaseHandler()
{
	Finalize();
}

void DatabaseHandler::Initialize()
{
}

void DatabaseHandler::Finalize()
{
	for (auto thread : m_threads)
	{
		thread->Stop();
		delete thread;
	}

	m_threads.clear();
}

void DatabaseHandler::InitThread(std::size_t _thread_count)
{
	m_threads.clear();
	m_threads.reserve(_thread_count);

	for (std::size_t i = 0; i < _thread_count; ++i)
	{
		m_threads.push_back(new Thread);
	}
}

void DatabaseHandler::Push(Job* _job)
{
	ThreadId_t thread_id = _job->owner->ThreadId() % m_threads.size();
	m_threads[thread_id]->Push(_job);
}
