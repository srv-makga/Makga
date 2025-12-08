module makga.network.endpoint;

#include <Winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <string>
#include <sstream>

namespace makga::network {
bool IPEndPoint::operator== (const IPEndPoint& _other) const
{
	if (type_ != _other.type_)
	{
		return false;
	}

	if (Port() != _other.Port())
	{
		return false;
	}

	if (true == IsV4())
	{
		return 0 == memcmp(&address_.ipv4, &_other.address_.ipv4, sizeof(address_.ipv4));
	}

	return 0 == ::memcmp(&address_.ipv6, &_other.address_.ipv6, sizeof(address_.ipv6));
}

IPEndPoint::IPEndPoint()
	: type_(None)
	, address_({})
{
}

IPEndPoint::IPEndPoint(unsigned long _ip, Port_t _port)
	: IPEndPoint()
{
	type_ = v4;
	address_.ipv4.sin_family = AF_INET;
	address_.ipv4.sin_addr.S_un.S_addr =  _ip ;
	address_.ipv4.sin_port = ::htons(_port);
}

IPEndPoint::IPEndPoint(const std::string& ip, Port_t port)
	: IPEndPoint()
{
	if (true == ip.empty())
	{
		return;
	}

	if (std::string::npos != ip.find("."))
	{
		type_ = v4;
		address_.ipv4.sin_family = AF_INET;
		address_.ipv4.sin_port = ::htons(port);
		::InetPtonA(AF_INET, ip.c_str(), &(address_.ipv4.sin_addr));
	}
	else
	{
		type_ = v6;
		address_.ipv6.sin6_family = AF_INET6;
		address_.ipv6.sin6_port = ::htons(port);

		::InetPtonA(AF_INET6, ip.c_str(), &(address_.ipv6.sin6_addr));
	}
}

IPEndPoint::IPEndPoint(const std::string& _ip, Port_t _port)
	: IPEndPoint()
{
	if (true == _ip.empty())
	{
		return;
	}

	if (std::string::npos != _ip.find("."))
	{
		type_ = v4;
		address_.ipv4.sin_family = AF_INET;
		address_.ipv4.sin_port = ::htons(_port);
		::InetPtonA(AF_INET, _ip.c_str(), &(address_.ipv4.sin_addr));
	}
	else
	{
		type_ = v6;
		address_.ipv6.sin6_family = AF_INET6;
		address_.ipv6.sin6_port = ::htons(_port);
		::InetPtonA(AF_INET6, _ip.c_str(), &(address_.ipv6.sin6_addr));
	}
}

IPEndPoint& IPEndPoint::operator=(const IPEndPoint& _other)
{
	if (this == &_other)
	{
		return *this;
	}

	type_ = _other.type_;
	address_ = _other.address_;
	return *this;
}

IPEndPoint& IPEndPoint::operator=(const SOCKADDR_IN& _sockaddr)
{
	type_ = v4;
	::memcpy(&address_.ipv4, &_sockaddr, sizeof(address_.ipv4));
	return *this;
}

IPEndPoint& IPEndPoint::operator=(const SOCKADDR_IN6& _sockaddr)
{
	type_ = v6;
	::memcpy(&address_.ipv6, &_sockaddr, sizeof(_sockaddr));
	return *this;
}

IPEndPoint::Type_t IPEndPoint::Type() const
{
	return type_;
}

bool IPEndPoint::IsV4() const
{
	return v4 == Type();
}

bool IPEndPoint::IsV6() const
{
	return v6 == Type();
}

const sockaddr_in& IPEndPoint::Addr() const
{
	return address_.ipv4;
}

const sockaddr_in6& IPEndPoint::Addr6() const
{
	return address_.ipv6;
}

std::string IPEndPoint::Ip() const
{
	static std::string empty_string;

	if (v4 == type_)
	{
		std::string ret;
		ret.resize(INET_ADDRSTRLEN);
		if (InetNtopA(AF_INET, &(address_.ipv4.sin_addr), const_cast<char*>(ret.c_str()), INET_ADDRSTRLEN) != nullptr)
		{
			return ret;
		}
	}

	else
	{
		std::string ret;
		ret.resize(INET6_ADDRSTRLEN);
		if (::InetNtopA(AF_INET6, &(address_.ipv6.sin6_addr), const_cast<char*>(ret.c_str()), INET6_ADDRSTRLEN) != nullptr)
		{
			return ret;
		}
	}
	
	return empty_string;
}

IPEndPoint::Port_t IPEndPoint::Port() const
{
	if (v4 == type_)
	{
		return ::ntohs(address_.ipv4.sin_port);
	}
	
	return ::ntohs(address_.ipv6.sin6_port);
}

std::string IPEndPoint::GetString() const
{
	std::stringstream ss;
	ss << " ip: " << Ip() << ", port: " << Port();
	return ss.str();
}
} // namespace makga::network