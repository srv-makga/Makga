#pragma once

#include "../Common/job_handler.h"
#include "pool.h"
#include <thread>
#include <functional>

class WorkerThread : public JobHandler
{
public:
	WorkerThread(ThreadId_t _id);
	virtual ~WorkerThread();

	bool Initialize();
	void Finalize();

	void Run(std::function<void(void)> _work);
	void Stop();

	void Push(Job_t _data) override;
	Job_t Pop() override;


	bool IsStop() const;
	bool IsEmpty() const;

protected:
	const ThreadId_t m_id;
	std::thread m_thread;
	std::atomic<bool> m_run;
	std::queue<Job_t> m_jobs;
	mutable std::recursive_mutex m_mutex;
	std::condition_variable_any m_cv;
};