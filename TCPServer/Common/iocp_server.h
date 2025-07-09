#pragma once

#include "../Core/iocp_service.h"
#include "../Core/iocp_session.h"
#include "../Core/iocp_acceptor.h"
#include "../Core/ip_endpoint.h"
#include <concurrent_unordered_map.h>
#include <concurrent_queue.h>

class IocpServer : public core::network::IocpService, public std::enable_shared_from_this<IocpServer>
{
public:
	using Service_t = core::network::IocpService;
	using Session_t = core::network::IocpSession;

public:
	IocpServer(std::shared_ptr<core::network::IocpCore> _core, const core::network::IPEndPoint& _endpoint);
	IocpServer(const IocpServer& _other) = delete;
	IocpServer(IocpServer&& _other) = delete;
	IocpServer& operator=(const IocpServer& _other) = delete;
	IocpServer& operator=(IocpServer&& _other) = delete;
	virtual ~IocpServer();

public: // IocpService
	bool Initialize() override;
	void Finalize() override;
	bool StartUp() override;
	bool StartUpEnd() override;
	bool Stop() override;
	const core::network::IPEndPoint& GetEndPoint() const override;
	std::size_t GetConnectCount() const override;
	std::size_t GetMaxConnectCount() const override;
	std::shared_ptr<Session_t> AllocSession() override;
	void DeallocSession(std::shared_ptr<Session_t> _session) override;
	bool AddSession(std::shared_ptr<core::network::IocpSession> _session) override;
	void RemoveSession(std::shared_ptr<core::network::IocpSession> _session) override;

public:
	void DisconnectAllSession();

protected:
	bool Run() override;

private:
	std::shared_ptr<core::network::IocpAcceptor> m_acceptor;
	core::network::IPEndPoint m_end_point;

	mutable core::RWMutex m_mutex_session;
	std::unordered_map<Session_t::Id, std::shared_ptr<Session_t>> m_sessions;
	std::queue<std::shared_ptr<Session_t>> m_free_sessions;
};