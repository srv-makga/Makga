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
	bool Initialize(std::shared_ptr<core::network::NetCore> _net_core) override;
	void Finalize() override;
	bool CreateThread(std::size_t _thread_count = 1) override;
	bool Start() override;
	void Stop() override;

protected:
	void IocpWork();
	void RioWork();

protected:
	std::atomic<bool> m_is_running;
	std::vector<std::thread> m_threads;
	std::shared_ptr<core::network::NetCore> m_net_core;
};
