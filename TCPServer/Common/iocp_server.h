#pragma once

#include "thread_manager.h"
#include "session_manager.hpp"
#include "../Core/iocp_service.h"
#include "../Core/iocp_session.h"
#include "../Core/iocp_listener.h"
#include "../Core/ip_endpoint.h"

class IocpServer : public core::network::IocpService, public std::enable_shared_from_this<IocpServer>
{
public:
	using Service_t = core::network::IocpService;
	using Session_t = std::shared_ptr<core::network::IocpSession>;
	using Thread_t = std::shared_ptr<std::thread>;

public:
	IocpServer(
		std::shared_ptr<core::network::IocpCore> _core,
		const core::network::IPEndPoint& _ep,
		SessionManager<Session_t>* _session_manager
	);
	IocpServer() = delete;
	IocpServer(const IocpServer& _other) = delete;
	IocpServer(IocpServer&& _other) = delete;
	IocpServer& operator=(const IocpServer& _other) = delete;
	IocpServer& operator=(IocpServer&& _other) = delete;
	virtual ~IocpServer();

public: // IocpService
	bool Initialize() override;
	void Finalize() override;
	bool Start() override;
	bool Stop() override;
	
	const core::network::IPEndPoint& GetEndPoint() const override;
	void SetEndPoint(const core::network::IPEndPoint& _ep) override;

private:
	bool RunServer(std::function<void(void)> _work);

private:
	std::shared_ptr<core::network::IocpListener> m_listener;
	core::network::IPEndPoint m_end_point;

	ThreadManager m_thread_manager;
};