#pragma once

#include "socket_base.h"

namespace core {
namespace network {
class SocketTcp : public SocketBase
{
public:
	using EndPoint_t = EndPoint;

	static constexpr int s_type = OS_DEF(SOCK_STREAM);
	static constexpr int s_protocol = OS_DEF(IPPROTO_TCP);

	static SocketTcp V4() noexcept
	{
		return SocketTcp(OS_DEF(AF_INET));
	}

	static SocketTcp V6() noexcept
	{
		return SocketTcp(OS_DEF(AF_INET6));
	}

public:
	SocketTcp() = delete;
	SocketTcp(int _family = OS_DEF(AF_INET)) noexcept;
	~SocketTcp();

	CoreError Create();
	CoreError Listen(int32_t _backlog) override;

	bool Linger(struct linger _linger) override;

	unsigned long Recv(char* _buffer, std::size_t _size) override;
	unsigned long Send(char* _buffer, std::size_t _size) override;
	unsigned long AsyncRecv(char* _buffer, std::size_t _size, OVERLAPPED* _overlapped) override;
	unsigned long AsyncSend(char* _buffer, std::size_t _size, OVERLAPPED* _overlapped) override;

private:
	int m_family;
};
} // namespace network
} // namespace core