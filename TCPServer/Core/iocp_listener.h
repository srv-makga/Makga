#pragma once

#include "core_header.h"
#include "iocp_event.h"
#include <memory>

class IocpServer;

namespace core {
namespace network {
class IocpListener
{
public:
	IocpListener(std::shared_ptr<IocpServer> _server);
	IocpListener() = delete;
	IocpListener(const IocpListener& _other) = delete;
	IocpListener(IocpListener&& _other) = delete;
	IocpListener& operator= (const IocpListener& _other) = delete;
	IocpListener& operator= (IocpListener&& _other) = delete;
	virtual ~IocpListener();

public:
	bool Initialize();
	void Finalize();

	bool StartAccept();
	void CloseSocket();

	void RegisterAccept(IocpAcceptEvent* _event);
	void ProcessAccept(IocpAcceptEvent* _event);

public:
	HANDLE GetHandle();
	void Dispatch(IocpEvent* _iocp_event, int bytesTransferred = 0);

private:
	SOCKET m_socket;
	std::vector<IocpAcceptEvent*> m_accept_events;
	std::shared_ptr<IocpServer> m_server;
};
} // namespace network
} // namespace core