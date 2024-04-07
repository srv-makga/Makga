#pragma once

#include "common_header.h"
#include "../Core/socket_tcp.h"

class TcpServer;

class TcpSession
{
public:
	TcpSession();
	virtual ~TcpSession();

public:
	void Initialize();
	void Finalize();

public:
	SessionId_t SessionId() const;
	void SetSessionId(SessionId_t _id);

	TcpServer* Server() const;
	void SetServer(TcpServer* _server);

	bool IsConnected() const;

	std::size_t BytesPending() const;
	std::size_t BytesSending() const;
	std::size_t BytesSent() const;
	std::size_t BytesReceived() const;

	std::size_t OptionRecvBufferSize() const;
	std::size_t OptionSendBufferSize() const;

private:
	SessionId_t m_id;
	TcpServer* m_server;

	core::network::SocketTcp m_socket;

	bool m_is_connected;

	std::size_t m_bytes_pending;
	std::size_t m_bytes_sending;
	std::size_t m_bytes_sent;
	std::size_t m_bytes_received;

	std::size_t m_option_recv_buffer_size;
	std::size_t m_option_send_buffer_size;
};