#pragma once

#include "net_handler.h"

class Job;
class NetThread : public NetHandler
{
public:
	NetThread();
	NetThread(const NetThread& _other) = delete;
	NetThread(NetThread&& _other) = delete;
	NetThread& operator=(const NetThread& _other) = delete;
	NetThread& operator=(NetThread&& _other) = delete;
	virtual ~NetThread();

public: // NetHandler
	bool Initialize(std::shared_ptr<core::network::NetCore> _net_core, std::shared_ptr<JobHandler> _job_handler) override;
	void Finalize() override;
	bool CreateThread(std::function<void()> _work, std::size_t _thread_count = 1) override; // 해당 함수 내용을 Initialize에 포함?
	bool Start() override;
	void Stop() override;

protected:
	void IocpWork();
	void RioWork();

protected:
	std::atomic<bool> m_is_running;
	std::function<void()> m_work;
	std::vector<std::thread> m_threads;

	std::shared_ptr<core::network::NetCore> m_net_core;
	std::shared_ptr<JobHandler> m_job_handler;
};
