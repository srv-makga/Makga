#pragma once

#include "core_header.h"
#include "iocp_event.h"
#include "iocp_object.h"
#include "connector.h"

namespace core {
namespace network {
class IocpService;
class IocpConnector : public Connector, public IocpObject
{
public:
	IocpConnector(std::shared_ptr<IocpService> _client);
	IocpConnector() = delete;
	IocpConnector(const IocpConnector& _other) = delete;
	IocpConnector(IocpConnector&& _other) = delete;
	IocpConnector& operator= (const IocpConnector& _other) = delete;
	IocpConnector& operator= (IocpConnector&& _other) = delete;
	virtual ~IocpConnector();

public:
	bool Initialize();
	void Finalize();

	bool Start();
	void Stop();
	void CloseSocket();

	void RegisterConnect(IocpConnectEvent* _event);
	void ProcessConnect(IocpConnectEvent* _event);

public: // IocpObject
	HANDLE GetHandle() const override;
	void Dispatch(IocpEvent* _iocp_event, int _bytes_transferred = 0) override;

protected:
	std::shared_ptr<IocpService> m_client;
	std::shared_ptr<IocpSession> m_session;
	IocpConnectEvent* m_connect_event;
};
} // namespace network
} // namespace core