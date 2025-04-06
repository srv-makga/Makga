#pragma once

#include "core_header.h"
#include "iocp_event.h"
#include "iocp_object.h"
#include "acceptor.h"

namespace core {
namespace network {
class IocpService;
class IocpAcceptor : public IocpObject, public Acceptor
{
public:
	IocpAcceptor(std::shared_ptr<IocpService> _server);
	IocpAcceptor() = delete;
	IocpAcceptor(const IocpAcceptor& _other) = delete;
	IocpAcceptor(IocpAcceptor&& _other) = delete;
	IocpAcceptor& operator= (const IocpAcceptor& _other) = delete;
	IocpAcceptor& operator= (IocpAcceptor&& _other) = delete;
	virtual ~IocpAcceptor();

public:
	bool Initialize();
	void Finalize();

	bool Start();
	void Stop();
	void CloseSocket();

	void RegisterAccept(IocpAcceptEvent* _event);
	void ProcessAccept(IocpAcceptEvent* _event);

public:
	HANDLE GetHandle() const override;
	void Dispatch(IocpEvent* _iocp_event, int _bytes_transferred = 0) override;

private:
	SOCKET m_socket;
	std::vector<IocpAcceptEvent*> m_accept_events;
	std::shared_ptr<IocpService> m_server;
};
} // namespace network
} // namespace core