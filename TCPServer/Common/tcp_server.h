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
	void Initialize();

	bool Start();
	void Stop();
	void Wait();

	bool Multicast(core::BufferPull& _buffer);
	bool Multicast(const char* _data, std::size_t _length);

	void RegisterSession(TcpSession* _session);
	void UnRegisterSession(SessionId_t _session_id);
	TcpSession* FindSession(SessionId_t _session_id);

	virtual TcpSession* AllocSession() = 0;
	virtual void DeAllocSession(TcpSession* _session) = 0;
	virtual bool DisconnectAll();

public:
	virtual void OnConnected(TcpSession* _session) {}
	virtual void OnDisconnected(TcpSession* _session) {}
	virtual void OnError(const std::string& _errmsg) {}
	
public:
	bool IsValid() const;

	// @brief 현재 서버에 연결된 세션 수
	std::size_t ConnectSessionCount() const;

	const core::network::EndPoint& EndPoint() const;

	std::size_t BytesPending() const;
	std::size_t BytesSent() const;
	std::size_t BytesReceived() const;

	int32_t OptionAcceptorBackLog() const;
	void SetOptionAcceptorBackLog(int32_t _value);

	std::size_t OptionReceiveBufferSize() const;
	std::size_t OptionSendBufferSize() const;

	bool OptionKeepAlive() const;
	void SetKeepAlive(bool _value);

protected:
	// @brief 서버 시작 중에 호출
	// @detail bind 이후 listen 이전
	virtual void OnStarting() = 0;

	// @brief 서버 시작 후에 호출
	// @detail listen 이후 호출
	virtual void OnStarted() = 0;

	// @brief 서버 정지 직후 호출
	// @detail accept flag만 해제하고 호출
	virtual void OnStopping() = 0;

	// @brief 소켓 종료 후 호출
	// @detail 
	virtual void OnStoped() = 0;

	// @brief accept 작업
	// @detail tcp session들을 accept 걸어준다
	virtual void StartAccept() = 0;

private:
	core::network::SocketTcp m_socket;

	bool m_is_valid;
	bool m_is_started;
	bool m_is_accepting;

	std::condition_variable m_exit_cv;
	mutable std::mutex m_exit_mutex;

	core::network::EndPoint m_endpoint;

	std::unordered_map<SessionId_t, TcpSession*> m_sessions;

	std::size_t m_bytes_pending;
	std::size_t m_bytes_sent;
	std::size_t m_bytes_received;

	std::size_t m_option_receive_buffer_size;
	std::size_t m_option_send_buffer_size;

	int32_t m_option_acceptor_backlog;
	bool m_option_keep_alive;
};