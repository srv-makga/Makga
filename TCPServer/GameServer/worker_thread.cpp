#include "pch.h"
#include "worker_thread.h"
#include "../Common/job.h"

WorkerThread::WorkerThread(ThreadId_t _id)
	: m_id(_id)
{
}

WorkerThread::~WorkerThread()
{
	Finalize();
}

bool WorkerThread::Initialize()
{
	return ThreadPool::Initialize();
}

void WorkerThread::Finalize()
{
	ThreadPool::Finalize();
}

bool WorkerThread::CreateThread(std::size_t _thread_count)
{
	ThreadPool::InitThread(_thread_count);
	return true;
}

void WorkerThread::Push(TaskType _job)
{
	ThreadPool::Push(std::forward<TaskType>(_job));
}