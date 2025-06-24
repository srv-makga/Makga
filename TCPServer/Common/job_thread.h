#pragma once

#include "job_handler.h"
#include "../Core/queue_priority.hpp"

class JobThread : public JobHandler
{
	using Queue_t = core::queue::ThreadSafePriorityQueue<std::shared_ptr<Job>>;

public:
	JobThread() = default;
	JobThread(const JobThread& _other) = delete;
	JobThread(JobThread&& _other) = delete;
	JobThread& operator=(const JobThread& _other) = delete;
	JobThread& operator=(JobThread&& _other) = delete;
	virtual ~JobThread() = default;

public: // JobHandler
	bool Initialize() override;
	void Finalize() override; 
	bool CreateThread(std::size_t _thread_count = 1) override; // 해당 함수 내용을 Initialize에 포함?
	void Push(std::shared_ptr<Job> _job) override;

protected:
	std::shared_ptr<Job> Pop() override;

protected:
	Queue_t m_job_queue;
	std::vector<std::thread> m_threads;
};
