module network.endpoint;

namespace makga::network {
bool IPEndPoint::operator== (const IPEndPoint& _other) const
{
	if (m_type != _other.m_type)
	{
		return false;
	}

	if (Port() != _other.Port())
	{
		return false;
	}

	if (true == IsV4())
	{
		return 0 == memcmp(&m_address.ipv4, &_other.m_address.ipv4, sizeof(m_address.ipv4));
	}

	return 0 == ::memcmp(&m_address.ipv6, &_other.m_address.ipv6, sizeof(m_address.ipv6));
}

IPEndPoint::IPEndPoint()
	: m_type(None)
	, m_address({})
{
}

IPEndPoint::IPEndPoint(unsigned long _ip, Port_t _port)
	: IPEndPoint()
{
	m_type = v4;
	m_address.ipv4.sin_family = AF_INET;
	m_address.ipv4.sin_addr.S_un.S_addr =  _ip ;
	m_address.ipv4.sin_port = ::htons(_port);
}

IPEndPoint::IPEndPoint(const std::string* _ip, Port_t _port)
	: IPEndPoint()
{
	if (nullptr == _ip)
	{
		return;
	}

	if (nullptr != _tcsstr(_ip, _T(".")))
	{
		m_type = v4;
		m_address.ipv4.sin_family = AF_INET;
		m_address.ipv4.sin_port = ::htons(_port);
		::InetPton(AF_INET, _ip, &(m_address.ipv4.sin_addr));
	}
	else
	{
		m_type = v6;
		m_address.ipv6.sin6_family = AF_INET6;
		m_address.ipv6.sin6_port = ::htons(_port);

		::InetPton(AF_INET6, _ip, &(m_address.ipv6.sin6_addr));
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
		m_type = v4;
		m_address.ipv4.sin_family = AF_INET;
		m_address.ipv4.sin_port = ::htons(_port);
		::InetPton(AF_INET, _ip.c_str(), &(m_address.ipv4.sin_addr));
	}
	else
	{
		m_type = v6;
		m_address.ipv6.sin6_family = AF_INET6;
		m_address.ipv6.sin6_port = ::htons(_port);
		::InetPton(AF_INET6, _ip.c_str(), &(m_address.ipv6.sin6_addr));
	}
}

IPEndPoint& IPEndPoint::operator=(const IPEndPoint& _other)
{
	if (this == &_other)
	{
		return *this;
	}

	m_type = _other.m_type;
	m_address = _other.m_address;
	return *this;
}

IPEndPoint& IPEndPoint::operator=(const SOCKADDR_IN& _sockaddr)
{
	m_type = v4;
	::memcpy(&m_address.ipv4, &_sockaddr, sizeof(m_address.ipv4));
	return *this;
}

IPEndPoint& IPEndPoint::operator=(const SOCKADDR_IN6& _sockaddr)
{
	m_type = v6;
	::memcpy(&m_address.ipv6, &_sockaddr, sizeof(_sockaddr));
	return *this;
}

IPEndPoint::Type_t IPEndPoint::Type() const
{
	return m_type;
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
	return m_address.ipv4;
}

const sockaddr_in6& IPEndPoint::Addr6() const
{
	return m_address.ipv6;
}

std::string IPEndPoint::Ip() const
{
	static std::string empty_string;

	if (v4 == m_type)
	{
		std::string ret;
		ret.resize(INET_ADDRSTRLEN);
		if (InetNtop(AF_INET, &(m_address.ipv4.sin_addr), const_cast<std::tchar*>(ret.c_str()), INET_ADDRSTRLEN) != nullptr)
		{
			return ret;
		}
	}

	else
	{
		std::string ret;
		ret.resize(INET6_ADDRSTRLEN);
		if (::InetNtop(AF_INET6, &(m_address.ipv6.sin6_addr), const_cast<std::tchar*>(ret.c_str()), INET6_ADDRSTRLEN) != nullptr)
		{
			return ret;
		}
	}
	
	return empty_string;
}

IPEndPoint::Port_t IPEndPoint::Port() const
{
	if (v4 == m_type)
	{
		return ::ntohs(m_address.ipv4.sin_port);
	}
	
	return ::ntohs(m_address.ipv6.sin6_port);
}

std::string IPEndPoint::GetString() const
{
	std::stringstream ss;
	ss << " ip: " << Ip() << ", port: " << Port();
	return ss.str();
}
} // namespace makga::network