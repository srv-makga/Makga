#pragma once

#include "core_header.h"
#include "string_function.h"

namespace core {
namespace network {
enum Address_e
{
	None,
	v4,
	v6
};
/*
@brief sockaddr_in, sockaddr_in6을 랩핑할 클래스
*/
class IPEndPoint
{
public:
	using Port_t = unsigned short;

	union Address_t {
		sockaddr_in ipv4;
		sockaddr_in6 ipv6;
	};

public:
	IPEndPoint() = delete;
	IPEndPoint(const std::tchar* _ip, Port_t _port);
	IPEndPoint(const std::tstring& _ip, Port_t _port);

	IPEndPoint& operator=(const IPEndPoint& _other);
	IPEndPoint& operator=(const SOCKADDR_IN& _sockaddr);
	IPEndPoint& operator=(const SOCKADDR_IN6& _sockaddr);

	bool IsV4() const;
	bool IsV6() const;

	const sockaddr_in& Addr() const;
	const sockaddr_in6& Addr6() const;

	Address_e Type() const;
	std::tstring Ip() const;
	Port_t Port() const;

private:
	Address_e m_type;
	Address_t m_address;
};
} // namespace network
} // namespace core