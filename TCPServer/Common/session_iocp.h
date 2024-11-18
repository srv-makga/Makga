#pragma once

#include "session_base.h"
#include "net_buffer.h"
#include "../Core/iocp_object.h"
#include "../Core/iocp_event.h"
#include "../Core/socket_tcp.h"
#include "../Core/iocp_service.h"
#include "../Core/buffer_ring.hpp"

class SessionIOCP : public SessionBase, core::network::IOCPObject
{
public:
	using Socket_t = core::network::SocketTcp;
	using Buffer_t = NetBuffer;
	using Packet_t = Packet;

	using SendEvent_t = core::network::IocpSendEvent<NetBuffer>;
	using RecvEvent_t = core::network::IocpRecvEvent;
	using ConnectEvent_t = core::network::IocpConnectEvent;
	using DisconnectEvent_t = core::network::IocpDisconnectEvent;

public:
	SessionIOCP();
	virtual ~SessionIOCP();

	void Initialize();
	void Finalize();

public:
	// 夸没何
	bool PostConnect();
	void PostDisconnect();
	void PostRecv();
	void PostSend();

	// 夸没狼 贸府 橇肺技胶
	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(std::size_t _bytes_transferred);
	void ProcessSend(std::size_t _bytes_transferred);

	bool IsConnect() const;

protected:
	virtual void OnConnected() {}
	virtual std::size_t OnRecv(char* buffer, std::size_t _length) final;
	virtual void OnRecvPacket(char* buffer, std::size_t len) {}
	virtual void OnSend(std::size_t len) {}
	virtual void OnDisconnected() {}

public: // JobOwner
	bool ProcPacket(NetPacket* _packet);
	ThreadId_t ThreadId() const;

public: // SessionBase
	void OnError(const std::string& _errmsg);
	void OnError(const std::exception& _exception);

protected:
	Socket_t m_socket;

	Buffer_t m_recv_buffer;

	core::RCMutex m_send_buffer_mutex;
	std::priority_queue<std::shared_ptr<Buffer_t>> m_send_buffer_queue;

	std::atomic<bool> m_is_connect;
	std::atomic<bool> m_is_send_registered;

	// iocp event
	SendEvent_t m_send_event;
	RecvEvent_t m_recv_event;
	ConnectEvent_t m_connect_event;
	DisconnectEvent_t m_disconnect_event;

	std::shared_ptr<core::network::IOCPService> m_iocp_service;

	std::size_t m_pending_bytes;
	std::size_t m_sending_bytes;
	std::size_t m_sent_bytes;
	std::size_t m_received_bytes;

	Time_t m_accept_time;
	Time_t m_disconnect_time;
	Time_t m_receive_time;
	Time_t m_send_time;
};