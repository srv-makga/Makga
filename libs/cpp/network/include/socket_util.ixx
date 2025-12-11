module;

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <mswsock.h>
//#pragma comment(lib, "ws2.lib")
#pragma comment(lib, "mswsock.lib")
#endif

export module makga.network.socket;

import makga.lib.pattern.singleton;

export namespace makga::network {
class SocketFunc : public makga::lib::Singleton<SocketFunc>
{
public:
	SocketFunc()
	{
		SOCKET s;
		InitFunc(s);
	}

	static void InitFunc(SOCKET socket)
	{
		DWORD bytes = 0;

		GUID acceptex_guid = WSAID_ACCEPTEX;
		::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &acceptex_guid, sizeof(acceptex_guid),
			&SocketFunc::Instance().AcceptEx, sizeof(LPFN_ACCEPTEX), &bytes, nullptr, nullptr);

		GUID acceptex_sock_addr_guid = WSAID_GETACCEPTEXSOCKADDRS;
		::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &acceptex_sock_addr_guid, sizeof(acceptex_sock_addr_guid),
			&SocketFunc::Instance().fn_acceptex_sock_addr, sizeof(LPFN_GETACCEPTEXSOCKADDRS), &bytes, nullptr, nullptr);

		GUID connectex_guid = WSAID_CONNECTEX;
		::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &connectex_guid, sizeof(connectex_guid),
			&SocketFunc::Instance().ConnectEx, sizeof(LPFN_CONNECTEX), &bytes, nullptr, nullptr);

		GUID disconnectex_guid = WSAID_DISCONNECTEX;
		::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &disconnectex_guid, sizeof(disconnectex_guid),
			&SocketFunc::Instance().DisconnectEx, sizeof(LPFN_DISCONNECTEX), &bytes, nullptr, nullptr);
	}

	inline static LPFN_ACCEPTEX AcceptEx = nullptr;
	inline static LPFN_GETACCEPTEXSOCKADDRS fn_acceptex_sock_addr = nullptr;
	inline static LPFN_CONNECTEX ConnectEx = nullptr;
	inline static LPFN_DISCONNECTEX DisconnectEx = nullptr;
};

export bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listen_socket)
{
	return SOCKET_ERROR != ::setsockopt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&listen_socket), sizeof(listen_socket));
}
} // makga::network
