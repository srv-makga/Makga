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
import makga.network.endpoint;

export namespace makga::network {
class SocketFunc : public makga::lib::Singleton<SocketFunc>
{
public:
	SocketFunc()
	{
		SOCKET s = 0;
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

#if defined(_WIN32)
	static SOCKET Socket()
	{
		return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	}

	static SOCKET RioSocket()
	{
		return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_REGISTERED_IO | WSA_FLAG_OVERLAPPED);
	}
#endif

	static void CloseSocket(SOCKET socket)
	{
		if (INVALID_SOCKET == socket)
		{
			return;
		}

		::shutdown(socket, SD_BOTH);
		::closesocket(socket);
		socket = INVALID_SOCKET;
	}

	static bool Bind(SOCKET socket, const makga::network::IPEndPoint& ep)
	{
		if (INVALID_SOCKET == socket)
		{
			return false;
		}

		const auto& addr = ep.Addr();

		return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&addr), sizeof(addr));
	}

	static bool BindAddrAny(SOCKET socket, unsigned short port)
	{
		SOCKADDR_IN address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = ::htonl(INADDR_ANY);
		address.sin_port = ::htons(port);

		return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&address), sizeof(address));
	}

	static bool Listen(SOCKET socket)
	{
		if (INVALID_SOCKET == socket)
		{
			return false;
		}

		return SOCKET_ERROR != ::listen(socket, SOMAXCONN);
	}

	static SOCKET Accept(SOCKET socket, OUT SOCKADDR_IN& addr)
	{
		if (INVALID_SOCKET == socket)
		{
			return INVALID_SOCKET;
		}

		int addr_length = sizeof(addr);
		return ::accept(socket, (SOCKADDR*)&addr, &addr_length);
	}

	static bool Connect(SOCKET socket, const IPEndPoint& endpoint)
	{
		if (INVALID_SOCKET == socket)
		{
			return false;
		}

		const auto& addr = endpoint.Addr();

		return SOCKET_ERROR != ::connect(socket, (SOCKADDR*)&addr, sizeof(addr));
	}

	static bool SetNodelay(SOCKET socket, bool optval)
	{
		return SetSockOpt<int>(socket, IPPROTO_TCP, TCP_NODELAY, optval);
	}

	static bool SetLinger(SOCKET socket, bool optval, int time)
	{
		linger templinger;
		templinger.l_onoff = optval;
		templinger.l_linger = time;

		return SetSockOpt<linger>(socket, SOL_SOCKET, SO_LINGER, templinger);
	}

	static bool SetSendBufSize(SOCKET _socket, int _size)
	{
		return SetSockOpt<char>(_socket, SOL_SOCKET, SO_SNDBUF, _size);
	}

	static bool SetRecvBufSize(SOCKET _socket, int _size)
	{
		return SetSockOpt<char>(_socket, SOL_SOCKET, SO_RCVBUF, _size);
	}

	static bool SetReuseAddr(SOCKET socket, bool optval)
	{
		return SetSockOpt<int>(socket, SOL_SOCKET, SO_REUSEADDR, optval);
	}

	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listen_socket)
	{
		return SetSockOpt<SOCKET>(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listen_socket);
	}

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
	{
		DWORD bytes = 0;
		return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), &bytes, NULL, NULL);
	}

	static bool BindRioFunctionTable(SOCKET socket, GUID guid, RIO_EXTENSION_FUNCTION_TABLE& fntable)
	{
		DWORD bytes = 0;
		return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), (void**)&fntable, sizeof(fntable), &bytes, NULL, NULL);
	}

	template<typename T>
	static inline bool SetSockOpt(SOCKET socket, int level, int optname, T optval)
	{
		if (INVALID_SOCKET == socket)
		{
			return false;
		}

		return SOCKET_ERROR != ::setsockopt(socket, level, optname, reinterpret_cast<char*>(&optval), sizeof(T));
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
