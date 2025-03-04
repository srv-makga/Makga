#pragma once

//#include "session_manager.hpp"
#include "../Core/rio_service.h"
#include "../Core/rio_core.h"
#include "../Core/rio_session.h"
#include "../Core/ip_endpoint.h"

class RioServer : public core::network::RioService, std::enable_shared_from_this<RioServer>
{
public:
	using Logic_t = std::function<void(void)>;

public:
	RioServer(core::network::IPEndPoint _end_point);

	RioServer() = delete;
	RioServer(const RioServer& other) = delete;
	RioServer(RioServer&& other) = delete;
	RioServer& operator=(const RioServer& other) = delete;
	RioServer& operator=(RioServer&& other) = delete;
	virtual ~RioServer();

public:
	bool Initialize() override;
	void Finalize() override;
	bool Start(std::function<void()> _func) override;
	bool Stop() override;

	bool InitServer();
	bool RunServer();

	void SetJobQueueLogic(Logic_t _func) { m_jobQueueLogicFunc = _func; }

	int GetConnectCnt() { return m_sessionCnt; }

private: // listener
	bool InitListener();
	bool StartListener();
	bool Accept();

private: // rioCore
	bool InitCore();
	bool StartCoreWork();

private:
	std::atomic<std::size_t> m_sessionCnt;

	SOCKET m_socket;
	core::network::IPEndPoint m_end_point;
	bool m_is_initialize;

	std::vector<std::shared_ptr<core::network::RioCore>> m_rio_cores;
	bool m_is_init_core;
	int m_core_count;
	int m_currAllocCoreNum;

	Logic_t m_jobQueueLogicFunc;

};