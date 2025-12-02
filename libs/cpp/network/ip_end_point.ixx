module;

#include <WinSock2.h>
#include <Windows.h>
#include <ws2ipdef.h>
#include <string>
#include <sstream>

export module network.endpoint;

export namespace makga::network {
class IPEndPoint
{
public:
	using Type_t = enum { None, v4, v6};
	using Port_t = unsigned short;
	using Address_t = union { sockaddr_in ipv4; sockaddr_in6 ipv6; };
	bool operator== (const IPEndPoint& _other) const;

public:
	IPEndPoint();
	IPEndPoint(unsigned long _ip, Port_t _port);
	IPEndPoint(unsigned long _ip, Port_t _port);
	IPEndPoint(const std::string& _ip, Port_t _port);

	IPEndPoint& operator=(const IPEndPoint& _other);
	IPEndPoint& operator=(const SOCKADDR_IN& _sockaddr);
	IPEndPoint& operator=(const SOCKADDR_IN6& _sockaddr);

	bool IsV4() const;
	bool IsV6() const;

	const sockaddr_in& Addr() const;
	const sockaddr_in6& Addr6() const;

	Type_t Type() const;
	std::string Ip() const;
	Port_t Port() const;

	std::string GetString() const;

private:
	Type_t m_type;
	Address_t m_address;
};
}