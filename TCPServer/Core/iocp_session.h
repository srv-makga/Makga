#pragma once

#include "session.h"
#include "ip_endpoint.h"
#include "io_context.hpp"
#include "iocp_object.h"
#include "iocp_event.h"
#include "lock.h"
#include "net_buffer.h"
#include <queue>

class Packet;

namespace core {
namespace network {
class IocpService;
class IocpSession : public Session, public IocpObject
{
public:
	IocpSession(ServiceType _type, std::size_t _buffer_size);
	virtual ~IocpSession();

public: // Session
	bool Initialize() override;
	void Finalize() override;

	Id GetSessionId() const override;
	void SetSessionId(Id _id) override;

	ServiceType GetSessionType() const override;
	void SetSessionType(ServiceType _type) override;

public:
	void Dispatch(IocpEvent* _iocp_event, int _bytes_transferred = 0) override;

	bool Connect();
	void Disconnect();
	void Send(std::shared_ptr<NetBuffer> _send_buffer);

	// @brief 동기 수신
	int Recv();

	bool RegisterConnect();
	void RegisterDisconnect();
	void RegisterZeroRecv();
	void RegisterRecv();
	void RegisterSend();

	void OnConnect();
	void OnDisconnect();
	void OnRecv(int _bytes_transferred);
	void OnSend(int _bytes_transferred);

	SOCKET GetSocket() const;
	const IPEndPoint& GetEndPoint() const;
	std::shared_ptr<IocpService> GetService() const;

	bool IsConnected() const;
	bool IsSendRegistered() const;

	void SetEndPoint(SOCKADDR_IN _addr);
	void SetEndPoint(IPEndPoint _ep);
	void SetService(std::shared_ptr<IocpService> _service);

protected:
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual std::size_t OnRecv(char* buffer, std::size_t _length);
	virtual void OnSend(std::size_t _length);
	virtual bool ProcPacket(std::shared_ptr<Packet> _packet) = 0;

public:
	Id m_id;
	ServiceType m_type;

	SOCKET m_socket;
	IPEndPoint m_end_point;

	std::atomic<bool> m_is_connected;
	bool m_is_zero_receivce;

	std::shared_ptr<NetBuffer> m_recv_buffer;

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