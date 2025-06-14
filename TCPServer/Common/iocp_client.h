#pragma once

#include "../Core/iocp_service.h"
#include "../Core/iocp_session.h"
#include "../Core/iocp_connector.h"
#include "../Core/ip_endpoint.h"

class IocpClient : public core::network::IocpService, public std::enable_shared_from_this<IocpClient>
{
public:
	using Service_t = core::network::IocpService;
	using Session_t = core::network::IocpSession;
	using Thread_t = std::shared_ptr<std::thread>;

public:
	IocpClient(std::shared_ptr<core::network::IocpCore> _core, std::shared_ptr<core::network::IPEndPoint>);
	IocpClient(const IocpClient& _other) = delete;
	IocpClient(IocpClient&& _other) = delete;
	IocpClient& operator=(const IocpClient& _other) = delete;
	IocpClient& operator=(IocpClient&& _other) = delete;
	virtual ~IocpClient();

public: // IocpService
	bool Initialize() override;
	void Finalize() override;
	bool StartUp() override;
	bool StartUpEnd() override;
	bool Stop() override;
	const std::shared_ptr<core::network::IPEndPoint> GetEndPoint() const override;
	std::size_t GetConnectCount() const override;
	std::size_t GetMaxConnectCount() const override;
	std::shared_ptr<core::network::IocpSession> AllocSession() override;
	void DeallocSession(std::shared_ptr<core::network::IocpSession> _session) override;
	bool AddSession(std::shared_ptr<core::network::IocpSession> _session) override;
	void RemoveSession(std::shared_ptr<core::network::IocpSession> _session) override;

public:
	void DisconnectAllSession();

protected:
	bool Send(std::shared_ptr<core::network::NetBuffer> _send_buffer, int _sequence = 0);
	bool Run(std::function<void(void)> _work) override;

private:
	std::shared_ptr<core::network::IocpConnector> m_connector;
	std::shared_ptr<core::network::IPEndPoint> m_end_point;

	std::atomic<bool> m_is_connected;

	mutable core::RWMutex m_mutex_session;
	std::shared_ptr<Session_t> m_session;
};