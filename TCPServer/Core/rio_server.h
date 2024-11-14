#pragma once

#include "service.h"
#include "rio_core.h"
#include "rio_session.h"
#include "ip_endpoint.h"
#include <memory>

namespace core {
namespace network {
class RioServer : public server::Service, std::enable_shared_from_this<RioServer>
{
public:
	using Session_t = std::shared_ptr<RioSession>;
	using Logic_t = std::function<void(void)>;

public:
	RioServer(IPEndPoint _ep);

	RioServer() = delete;
	RioServer(const RioServer& other) = delete;
	RioServer(RioServer&& other) = delete;
	RioServer& operator=(const RioServer& other) = delete;
	RioServer& operator=(RioServer&& other) = delete;
	virtual ~RioServer();

	virtual bool Start(Logic_t _logic) override;
	virtual bool Stop() override;

	bool InitServer();
	bool RunServer();

	void SetJobQueueLogic(Logic_t _func) { m_jobQueueLogicFunc = _func; }

	int GetConnectCnt() { return m_sessionCnt; }

private:
	// listener
	bool InitListener();
	bool StartListener();
	bool Accept();

	// rioCore
	bool InitCore();
	bool StartCoreWork();

private:
	std::atomic<std::size_t> m_sessionCnt;

	// Listener
	SOCKET m_listener;
	IPEndPoint m_ep;
	bool m_bInitListener;

	std::vector<std::shared_ptr<RioCore>> m_rio_cores;
	bool m_bInitCore;
	int m_coreCnt;
	int m_currAllocCoreNum;

	Logic_t m_jobQueueLogicFunc;

protected:
};
} // namespace network
} // namespace core