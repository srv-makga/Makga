#pragma once

#include "session.h"
#include "ip_endpoint.h"
#include "io_context.hpp"
#include "iocp_object.h"
#include "iocp_event.h"
#include "lock.h"
#include "net_buffer.h"
#include <queue>

namespace core {
namespace network {
class IocpService;
class IocpSession : public Session, public IocpObject
{
public:
	IocpSession();
	virtual ~IocpSession();

public:
	bool Initialize();
	void Finalize();

	void Dispatch(IocpEvent* _iocp_event, int _bytes_transferred = 0) override;

	bool Connect();
	void Disconnect();
	void Send(std::shared_ptr<NetBuffer> _send_buffer);

	bool RegisterConnect();
	void RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int _bytes_transferred);
	void ProcessSend(int _bytes_transferred);

	SOCKET GetSocket() const;
	const IPEndPoint& GetEndPoint() const;
	std::shared_ptr<IocpService> GetService() const;

	bool IsConnected() const;
	bool IsSendRegistered() const;

	void SetEndPoint(SOCKADDR_IN _addr);
	void SetEndPoint(IPEndPoint _ep);
	void SetService(std::shared_ptr<IocpService> _service);

public: // Session
	Id SessionId() const override { return 0; }// @todo ±¸Çö

protected:
	virtual void OnConnected() = 0;
	virtual int OnRecv(char* buffer, int len) = 0;
	virtual void OnRecvPacket(char* buffer, int len) = 0;
	virtual void OnSend(int len) = 0;
	virtual void OnDisconnected() = 0;

public:
	SOCKET m_socket;
	IPEndPoint m_end_point;
	std::atomic<bool> m_is_connected;

	NetBuffer m_recv_buffer;
	core::RWMutex m_send_mutex;
	std::queue<std::shared_ptr<NetBuffer>> m_send_buffer_queue;
	std::atomic<bool> m_is_send_registered;

	IocpRecvEvent m_recv_event;
	IocpConnectEvent m_connect_event;
	IocpDisconnectEvent m_disconnect_event;
	IocpSendEvent<NetBuffer> m_send_event;

	std::shared_ptr<IocpService> m_service;
};
}// namespace network
}// namespace core