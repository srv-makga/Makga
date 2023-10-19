#include "pch.h"
#include "database_thread.h"

void DatabaseThread::Run()
{
	while (true)
	{
		Job* job = m_queue.Pop();
		if (false == m_client.ProcPacket(reinterpret_cast<SessionBase*>(job->owner), job->packet))
		{
			LOG_ERROR << "DBClient proc packet fail";
		}

		Job::Push(job);
	}
}

void DatabaseThread::Push(Job* _job)
{
	m_queue.Push(_job);
}