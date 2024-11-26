#pragma once

#include "core_header.h"
#include "iocp_event.h"
#include <memory>

namespace core {
namespace network {
class IocpService;
class IocpListener : public std::enable_shared_from_this<IocpListener>
{
public:
	IocpListener(std::shared_ptr<IocpService> _server);
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
	void Dispatch(IocpEvent* _event, int _bytes_transferred = 0);

private:
	SOCKET m_socket;
	std::vector<IocpAcceptEvent*> m_accept_events;
	std::shared_ptr<IocpService> m_server;
};
} // namespace network
} // namespace core