#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <mswsock.h>
#pragma comment(lib, "ws2.lib")
#pragma comment(lib, "mswsock.lib")
#endif

module makga.network.service;

export namespace makga::network {
NetService::NetService(NetServiceType type)
	: service_type_(type)
{
	WSADATA wsa_data{};
	::WSAStartup(MAKEWORD(2, 2), &wsa_data);
}

NetService::~NetService()
{
	::WSACleanup();
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