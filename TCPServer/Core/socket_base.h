#pragma once

#include "socket_header.h"
#include "ip_endpoint.h"
#include "buffer_base.h"
#include <string>
#include <optional>

namespace core {
namespace network {
Socket_t get_socket_async(int _family, int _type, int _protocol);
#ifdef _WIN32
Socket_t CreateRegisteredIOSocket();
#endif
int socket_no_blocking(Socket_t _socket);
int reuse(Socket_t _socket);

// Protocol TCP, UDP, ETC...
class SocketBase
{
public:
	enum State
	{
		Invalid,
		Open,
		Terminate,
	};

	SocketBase();
	~SocketBase();

	void Initialize();
	void Finalize();

	// @param _family AF_INET, AF_INET6
	// @param _type SOCK_STREAM, SOCK_DGRAM
	// @param _protocol IPPROTO_TCP, IPPROTO_UDP
	virtual CoreError Create(int _family, int _type, int _protocol);
	virtual void Close();

	virtual CoreError Listen(int32_t _backlog) const { return SocketFailListen; }
	bool Bind(const IPEndPoint& _endpoint);

	bool Connect(const IPEndPoint& _endpoint);

	// @brief 소켓의 송신, 수신 또는 양쪽 끝을 종료합니다.
	int Shutdown(Shutdown_e _what);

	// @brief 소켓에서 처리되지 않은 비동기 작업을 취소합니다.
	void Cancel(); // @todo

	bool Reuse();

	bool Pool(long _msec);

	bool OptionRecvBufferSize(int _buffer_size);
	bool OptionSendBufferSize(int _buffer_size);

	// @brief 소켓의 데이터를 일정 크기로 모아서 전송
	// @detail tcp only
	bool OptionNagle(bool _value);

	// @brief 소켓 종료시
	// @detail tcp only
	virtual bool Linger(struct linger _linger) { return true; }


	virtual unsigned long Recv(char* _buffer, std::size_t _size) = 0;
	virtual unsigned long Send(char* _buffer, std::size_t _size) = 0;
	virtual unsigned long AsyncRecv(char* _buffer, std::size_t _size, OVERLAPPED* _overlapped) = 0;
	virtual unsigned long AsyncSend(char* _buffer, std::size_t _size, OVERLAPPED* _overlapped) = 0;

	SocketBase& operator= (SOCKET _socket);
	operator SOCKET() const;
	 
public:
	inline SOCKET Socket() const { return m_socket; }
	inline void Socket(SOCKET _socket) { m_socket = _socket; }

	inline int Family() const { return m_family; }
	inline void Family(int _family) { m_family = _family; }

	inline bool IsValid() const { return m_is_valid; }
	inline void SetValid(bool _value) { m_is_valid = _value; }

	inline bool IsTerminate() const { return Terminate == m_state; }

	inline bool NativeNonBlocking() const { return m_native_non_blocking; }
	inline void SetNativeNonBlocking(bool _native_non_blocking) { m_native_non_blocking = _native_non_blocking; }

	inline const IPEndPoint& RemoteAddr() const { return m_remote_addr; }
	inline void SetRemoteAddr(const IPEndPoint& _remote_addr) { m_remote_addr = _remote_addr; }
	inline void SetRemoteAddr(const SOCKADDR_IN* _sockaddr) { m_remote_addr = *_sockaddr; }
	inline void SetRemoteAddr(const SOCKADDR_IN6* _sockaddr) { m_remote_addr = *_sockaddr; }

	inline const IPEndPoint& LocalAddr() const { return m_local_addr; }
	inline void SetLocalAddr(const IPEndPoint& _local_addr) { m_local_addr = _local_addr; }
	inline void SetLocalAddr(const SOCKADDR_IN* _sockaddr) { m_local_addr = *_sockaddr; }
	inline void SetLocalAddr(const SOCKADDR_IN6* _sockaddr) { m_local_addr = *_sockaddr; }

protected:
	Socket_t m_socket;
	int m_family;
	bool m_is_valid;
	bool m_native_non_blocking;
	State m_state;

	IPEndPoint m_remote_addr;
	IPEndPoint m_local_addr;
};
} // namespace network
} // namespace core;