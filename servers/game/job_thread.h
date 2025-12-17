#pragma once

#include <memory>

import makga.network.jobhandler;
import makga.lib.doublequeue;
import makga.lib.thread.group;

using Job = makga::network::Job;

class JobThread : public makga::network::JobHandler
{
public:
	JobThread();
	virtual ~JobThread();

	bool Initialize();
	void Finalize();

public: // JobHandler
	bool CreateThread(std::size_t thread_count = 1) override;
	bool Start() override;
	void Stop() override;

	void Push(std::shared_ptr<Job> job) override;

protected: // JobHandler
	std::shared_ptr<Job> Pop() override;

protected:
	std::size_t thread_count_;
	makga::lib::ThreadGroup thread_group_;
	makga::lib::DoubleQueue<std::shared_ptr<Job>> job_queue_;
};