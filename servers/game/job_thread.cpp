#include "pch.h"
#include "job_thread.h"

import makga.network.jobhandler;
import makga.lib.doublequeue;

JobThread::JobThread()
	: thread_count_(1)
{
}

JobThread::~JobThread()
{
}

bool JobThread::Initialize()
{
	return true;
}

void JobThread::Finalize()
{
	thread_group_.Stop();
}

bool JobThread::CreateThread(std::size_t thread_count)
{
	thread_count_ = thread_count;
	return true;
}

bool JobThread::Start()
{
	thread_group_.Start([this]() {
		while (true)
		{
			auto job = Pop();
			if (nullptr == job)
			{
				continue;
			}
			job->Execute();
		}
	}, thread_count_);

	return true;
}

void JobThread::Stop()
{
	thread_group_.Stop();
}

void JobThread::Push(std::shared_ptr<Job> job)
{
	job_queue_.Push(job);
}

std::shared_ptr<Job> JobThread::Pop()
{
	return job_queue_.Pop();
}