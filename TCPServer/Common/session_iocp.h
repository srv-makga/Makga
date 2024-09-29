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

	using SendEvent_t = core::network::IOCPSendEvent<NetBuffer>;
	using RecvEvent_t = core::network::IOCPRecvEvent;
	using ConnectEvent_t = core::network::IOCPConnectEvent;
	using DisconnectEvent_t = core::network::IOCPDisconnectEvent;

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
	// override at the content level
	virtual void OnConnected() {}
	virtual std::size_t OnRecv(char* buffer, std::size_t _len);
	virtual void OnRecvPacket(char* buffer, std::size_t len) {}
	virtual void OnSend(std::size_t len) {}
	virtual void OnDisconnected() {}

public: // JobOwner
	bool ProcPacket(NetPacket* _packet);
	ThreadId_t ThreadId() const;

public: // SessionBase
	bool RecvPacket(NetPacket* packet);
	void OnError(const std::string& _errmsg);
	void OnError(std::exception& _exception);

	void Send(Packet_t _packet);



private:
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

	std::shared_ptr<core::network::IOCPService> m_iocpService;

	std::size_t m_pending_bytes;
	std::size_t m_sending_bytes;
	std::size_t m_sent_bytes;
	std::size_t m_received_bytes;

	Time_t m_accept_time;
	Time_t m_disconnect_time;
	Time_t m_receive_time;
	Time_t m_send_time;
};