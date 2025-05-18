#pragma once

#include "thread_manager.h"
#include "session_manager.hpp"
#include "../Core/iocp_service.h"
#include "../Core/iocp_session.h"
#include "../Core/iocp_connector.h"
#include "../Core/ip_endpoint.h"

class IocpClient : public core::network::IocpService, public std::enable_shared_from_this<IocpClient>
{
public:
	using Service_t = core::network::IocpService;
	using Session_t = std::shared_ptr<core::network::IocpSession>;
	using Thread_t = std::shared_ptr<std::thread>;

public:
	IocpClient();
	IocpClient(const IocpClient& _other) = delete;
	IocpClient(IocpClient&& _other) = delete;
	IocpClient& operator=(const IocpClient& _other) = delete;
	IocpClient& operator=(IocpClient&& _other) = delete;
	virtual ~IocpClient();

public: // IocpService
	bool Initialize() override;
	void Finalize() override;
	bool Start() override;
	bool Stop() override;

	std::size_t GetMaxSessionCount() const override;

	const core::network::IPEndPoint& GetEndPoint() const override;
	void SetEndPoint(const core::network::IPEndPoint& _ep) override;

public:
	bool Setup(std::shared_ptr<core::network::IocpCore> _core,
		const core::network::IPEndPoint& _ep,
		std::function<std::shared_ptr<core::network::IocpSession>(void)> _alloc_func,
		std::function<void(std::shared_ptr<core::network::IocpSession>)> _dealloc_func
	);

private:
	bool RunClient(std::function<void(void)> _work);

private:
	std::shared_ptr<core::network::IocpConnector> m_connector;
	core::network::IPEndPoint m_end_point;
	bool m_is_setup;

	ThreadManager m_thread_manager;
};