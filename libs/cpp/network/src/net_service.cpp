module;

#if defined(_WIN32)
#include <winsock2.h>
#include <MSWSock.h>
#include <ws2tcpip.h>
#include <windows.h>
//#pragma comment(lib, "ws2.lib")
//#pragma comment(lib, "mswsock.lib")
#endif

module makga.network.service;

namespace makga::network {
NetService::NetService(NetServiceType type)
	: service_type_(type)
{
#if defined(_WIN32)
	WSADATA wsa_data{};
	(void)::WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif
}

NetService::~NetService()
{
#if defined(_WIN32)
	::WSACleanup();
#endif
}

bool NetService::Start()
{
	if (false == StartUp())
	{
		return false;
	}

	return StartUpEnd();
}

NetServiceType NetService::GetServiceType() const
{
	return service_type_;
}
} // namespace makga::network
