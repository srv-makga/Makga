#pragma once

#include "common_type.h"
#include "../Core/session.h"

class NetHandler;
class JobHandler;

struct NetActorConfig
{
	std::string ip;
	uint16_t port;
};

// @brief Acceptor, Connector의 인터페이스
class NetActor
{
public:
	using Session_t = core::network::Session;

public:
	NetActor()
		: m_net_handler(nullptr)
		, m_job_handler(nullptr)
	{
	}
	virtual ~NetActor() {}

	virtual bool Initialize()
	{
		m_net_handler = nullptr;
		m_job_handler = nullptr;
	}

	virtual void Finalize() = 0;

	virtual void InitHandler(std::shared_ptr<NetHandler> _net_handler, std::shared_ptr<JobHandler> _job_handler)
	{
		m_net_handler = _net_handler;
		m_job_handler = _job_handler;
	}

	virtual bool OnAccepted(std::shared_ptr<Session_t>, IOContext_t*) = 0;
	virtual bool OnConnected(std::shared_ptr<Session_t>, IOContext_t*) = 0;
	virtual bool OnReceived(std::shared_ptr<Session_t>, DWORD, IOContext_t*) = 0;
	virtual bool OnSent(std::shared_ptr<Session_t>, DWORD, IOContext_t*) = 0;
	virtual bool OnClosed(std::shared_ptr<Session_t>, IOContext_t*) = 0;

	std::shared_ptr<NetHandler> GetNetHandler() const { return m_net_handler; }
	std::shared_ptr<JobHandler> GetJobHandler() const { return m_job_handler; }

protected:
	// @detail Select, IOCP 등 종류가 있다
	std::shared_ptr<NetHandler> m_net_handler;
	// @detail logic thread
	std::shared_ptr<JobHandler> m_job_handler;
};