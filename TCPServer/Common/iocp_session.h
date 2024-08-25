#pragma once

#include "session.h"
#include "../Core/socket_tcp.h"
#include "../Core/iocp_context.h"
#include "../Core/buffer_ring.h"

// @todo IOCPContext ÇÊ¿ä

class IOCPSession : public ISession
{
public:
	using Socket_t = core::network::SocketTcp;
	using Buffer_t = core::BufferRing;
	using Context_t = std::shared_ptr<core::network::IOCPContext>;

public:
	IOCPSession(SessionId_t _id);
	virtual ~IOCPSession();

	bool Initialize() override;
	void FInalize() override;

public:
	bool PostAccept(SOCKET _socket) override;
	bool PostConnect(const core::network::IPEndPoint& _endpoint) override;
	bool PostDisconnect() override;
	bool PostZeroRecv() override;
	bool PostRecv() override;
	bool PostSend() override;

	void OnAccept(IOContext_t* _context) override;
	void OnConnect() override;
	void OnDisconnect() override;
	void OnRecv(DWORD _bytes_transferred, IOContext_t* _context) override;
	void OnSent(DWORD _bytes_transferred) override;

public:
	SessionId_t SessionId() const override { return m_session_id; }

private:
	SessionId_t m_session_id;
	Socket_t m_socket;

	std::atomic<bool> m_is_connected;

	std::size_t m_pending_bytes;
	std::size_t m_sending_bytes;
	std::size_t m_sent_bytes;
	std::size_t m_received_bytes;

	Time_t m_accept_time;
	Time_t m_disconnect_time;
	Time_t m_receive_time;
	Time_t m_send_time;
};