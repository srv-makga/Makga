module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
using SOCKADDR_IN = sockaddr_in;
using SOCKADDR_IN6 = sockaddr_in6;
#endif

#include <cstring>
#include <sstream>
#include <string>

module makga.network.endpoint;

namespace makga::network {
bool IPEndPoint::operator==(const IPEndPoint& other) const
{
    if (type_ != other.type_ || Port() != other.Port())
        return false;
    if (IsV4())
        return std::memcmp(&address_.ipv4, &other.address_.ipv4, sizeof(address_.ipv4)) == 0;
    return std::memcmp(&address_.ipv6, &other.address_.ipv6, sizeof(address_.ipv6)) == 0;
}

IPEndPoint::IPEndPoint()
    : type_(None)
    , address_({})
{
}

IPEndPoint::IPEndPoint(unsigned long ip, Port_t port)
    : IPEndPoint()
{
    type_ = v4;
    address_.ipv4.sin_family = AF_INET;
#ifdef _WIN32
    address_.ipv4.sin_addr.S_un.S_addr = ip;
#else
    address_.ipv4.sin_addr.s_addr = ip;
#endif
    address_.ipv4.sin_port = ::htons(port);
}

IPEndPoint::IPEndPoint(const std::string& ip, Port_t port)
    : IPEndPoint()
{
    if (ip.empty())
        return;

    if (ip.find('.') != std::string::npos)
    {
        type_ = v4;
        address_.ipv4.sin_family = AF_INET;
        address_.ipv4.sin_port = ::htons(port);
#ifdef _WIN32
        if (::InetPtonA(AF_INET, ip.c_str(), &address_.ipv4.sin_addr) != 1)
#else
        if (::inet_pton(AF_INET, ip.c_str(), &address_.ipv4.sin_addr) != 1)
#endif
            type_ = None;
    }
    else
    {
        type_ = v6;
        address_.ipv6.sin6_family = AF_INET6;
        address_.ipv6.sin6_port = ::htons(port);
#ifdef _WIN32
        if (::InetPtonA(AF_INET6, ip.c_str(), &address_.ipv6.sin6_addr) != 1)
#else
        if (::inet_pton(AF_INET6, ip.c_str(), &address_.ipv6.sin6_addr) != 1)
#endif
            type_ = None;
    }
}

IPEndPoint& IPEndPoint::operator=(const IPEndPoint& other)
{
    if (this != &other)
    {
        type_ = other.type_;
        address_ = other.address_;
    }
    return *this;
}

IPEndPoint& IPEndPoint::operator=(const SOCKADDR_IN& sockaddr)
{
    type_ = v4;
    std::memcpy(&address_.ipv4, &sockaddr, sizeof(address_.ipv4));
    return *this;
}

IPEndPoint& IPEndPoint::operator=(const SOCKADDR_IN6& sockaddr)
{
    type_ = v6;
    std::memcpy(&address_.ipv6, &sockaddr, sizeof(address_.ipv6));
    return *this;
}

IPEndPoint::Type_t IPEndPoint::Type() const
{
    return type_;
}

bool IPEndPoint::IsV4() const
{
    return type_ == v4;
}

bool IPEndPoint::IsV6() const
{
    return type_ == v6;
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
    char text[INET6_ADDRSTRLEN]{};
    const void* address = nullptr;
    int family = AF_UNSPEC;
    if (type_ == v4)
    {
        family = AF_INET;
        address = &address_.ipv4.sin_addr;
    }
    else if (type_ == v6)
    {
        family = AF_INET6;
        address = &address_.ipv6.sin6_addr;
    }
    else
    {
        return {};
    }

#ifdef _WIN32
    return ::InetNtopA(family, const_cast<void*>(address), text, static_cast<DWORD>(sizeof(text))) != nullptr
        ? std::string(text) : std::string{};
#else
    return ::inet_ntop(family, address, text, sizeof(text)) != nullptr
        ? std::string(text) : std::string{};
#endif
}

IPEndPoint::Port_t IPEndPoint::Port() const
{
    if (type_ == v4)
        return ::ntohs(address_.ipv4.sin_port);
    if (type_ == v6)
        return ::ntohs(address_.ipv6.sin6_port);
    return 0;
}

std::string IPEndPoint::GetString() const
{
    std::stringstream stream;
    stream << " ip: " << Ip() << ", port: " << Port();
    return stream.str();
}

} // namespace makga::network
