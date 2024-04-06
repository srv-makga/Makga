#pragma once

#include "common_header.h"
#include "../Core/socket_tcp.h"
#include "../Core/endpoint.h"
#include "../Core/buffer_pull.h"

class TcpSession;

class TcpServer
{
public:
	TcpServer() = delete;
	TcpServer(const std::string& _ip, Port_t _port);
	TcpServer(const core::network::EndPoint& _ep);
	virtual ~TcpServer();

public:
	bool Start();
	bool Stop();
	void Wait();

	bool Multicast(core::BufferPull& _buffer);
	bool Multicast(const char* _data, std::size_t _length);

	void RegisterSession(TcpSession* _session);
	void UnRegisterSession(SessionId_t _session_id);
	TcpSession* FindSession(SessionId_t _session_id);

	virtual TcpSession* AllocSession() = 0;
	virtual void DeAllocSession(TcpSession* _session) = 0;
	virtual bool DisConnectAll();

public:
	virtual void OnConnected(TcpSession* _session) {}
	virtual void OnDisconnected(TcpSession* _session) {}
	virtual void OnError(const std::string& _errmsg) {}
	
public:
	bool IsValid() const;

	// @brief 현재 서버에 연결된 세션 수
	std::size_t ConnectSessionCount() const;

	const core::network::EndPoint& EndPoint() const;

	int32_t BytesPending() const;
	int32_t BytesSent() const;
	int32_t BytesReceived() const;

	int32_t OptionAcceptorBackLog() const;
	void SetOptionAcceptorBackLog(int32_t _value);

	bool OptionKeepAlive() const;
	void SetKeepAlive(bool _value);

private:
	core::network::SocketTcp m_socket;

	bool m_is_valid;

	std::condition_variable m_exit_cv;
	std::mutex m_exit_mutex;

	core::network::EndPoint m_endpoint;

	std::unordered_map<SessionId_t, TcpSession*> m_sessions;

	int32_t m_bytes_pending;
	int32_t m_bytes_sent;
	int32_t m_bytes_received;

	int32_t m_option_acceptor_backlog;
	bool m_option_keep_alive;
};