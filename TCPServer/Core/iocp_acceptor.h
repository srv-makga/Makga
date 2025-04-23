#pragma once

#include "core_header.h"
#include "iocp_event.h"
#include "iocp_object.h"
#include "acceptor.h"
#include "lock.h"

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

	bool RegisterAccept(IocpAcceptEvent* _event);
	void ProcessAccept(IocpAcceptEvent* _event);

	bool CanAcceptSession() const;

public: // IocpObject
	HANDLE GetHandle() const override;
	void Dispatch(IocpEvent* _iocp_event, int _bytes_transferred = 0) override;

public: // Acceptor
	std::size_t GetCurConnectCount() const override;
	std::size_t GetMaxConnectCount() const override; // @todo 구현

protected:
	void PushFreeAcceptEvent(IocpAcceptEvent* _event);
	IocpAcceptEvent* PopFreeAcceptEvent();

private:
	SOCKET m_socket;

	mutable core::RCMutex m_mutex;
	std::vector<IocpAcceptEvent*> m_accept_events; // @todo free, using 구분 필요
	std::queue< IocpAcceptEvent*> m_free_accept_events;
	std::shared_ptr<IocpService> m_server;
};
} // namespace network
} // namespace core