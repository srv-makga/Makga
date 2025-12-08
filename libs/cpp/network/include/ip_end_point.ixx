module;

#include <WinSock2.h>
#include <Windows.h>
#include <ws2ipdef.h>
#include <stdio.h>
#include <sstream>

export module makga.network.endpoint;

import <string>;

export namespace makga::network {
class IPEndPoint
{
public:
	using Type_t = enum { None, v4, v6};
	using Port_t = unsigned short;
	using Address_t = union { sockaddr_in ipv4; sockaddr_in6 ipv6; };
	bool operator== (const IPEndPoint& other) const;

public:
	IPEndPoint();
	IPEndPoint(unsigned long ip, Port_t port);
	IPEndPoint(const std::string& ip, Port_t port);

	IPEndPoint& operator=(const IPEndPoint& other);
	IPEndPoint& operator=(const SOCKADDR_IN& sockaddr);
	IPEndPoint& operator=(const SOCKADDR_IN6& sockaddr);

	bool IsV4() const;
	bool IsV6() const;

	const sockaddr_in& Addr() const;
	const sockaddr_in6& Addr6() const;

	Type_t Type() const;
	std::string Ip() const;
	Port_t Port() const;

	std::string GetString() const;

private:
	Type_t type_;
	Address_t address_;
};
} // namespace makga::network