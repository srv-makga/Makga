#pragma once

#include "socket_base.h"

namespace core {
namespace network {
class SocketUdp : public SocketBase
{
public:
	static constexpr int s_type = OS_DEF(SOCK_DGRAM);
	static constexpr int s_protocol = OS_DEF(IPPROTO_UDP);

	static SocketUdp V4() noexcept
	{
		return SocketUdp(OS_DEF(AF_INET));
	}

	static SocketUdp V6() noexcept
	{
		return SocketUdp(OS_DEF(AF_INET6));
	}

	SocketUdp() = delete;
	SocketUdp(int _family) noexcept;
	~SocketUdp();

	unsigned long Recv(char* _buffer, std::size_t _size) override;
	unsigned long Send(char* _buffer, std::size_t _size) override;
	unsigned long AsyncRecv(char* _buffer, std::size_t _size, OVERLAPPED* _overlapped) override;
	unsigned long AsyncSend(char* _buffer, std::size_t _size, OVERLAPPED* _overlapped) override;

private:
	int m_family;
};
} // namespace network
} // namespace core