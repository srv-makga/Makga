#pragma once

#include "net_header.h"

namespace core {
namespace network {
class SocketCore
{
public:
	SocketCore();
	SocketCore(const SocketCore&) = delete;
	SocketCore(SocketCore&&) = delete;
	SocketCore& operator=(const SocketCore&) = delete;
	SocketCore& operator=(SocketCore&&) = delete;
	~SocketCore();

	static void Initialize();

	static SOCKET Socket();
	static SOCKET RioSocket();

	static bool SetNodelay(SOCKET _socket, bool _optval);
	static bool SetLinger(SOCKET _socket, bool _optval, int _time);
	static bool SetSendBufSize(SOCKET _socket, int _size);
	static bool SetRecvBufSize(SOCKET _socket, int _size);
	static bool SetReuseAddr(SOCKET _socket, bool _optval);
	static bool SetUpdateAcceptSocket(SOCKET _socket, SOCKET _listenSocket);

	static int Send(SOCKET socket, char* buf, int size);
	static int Recv(SOCKET socket, char* buf, int size);

private:
	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static bool BindRioFunctionTable(SOCKET socket, GUID guid, RIO_EXTENSION_FUNCTION_TABLE& fnTable);

	template<typename T>
	inline static bool SetSockOpt(SOCKET _socket, int _level, int _optname, T _optval);

public:
	static LPFN_ACCEPTEX fn_acceptex;
	static LPFN_GETACCEPTEXSOCKADDRS fn_acceptex_sock_addr;
	static LPFN_CONNECTEX fn_connectex;
	static LPFN_DISCONNECTEX fn_disconnectex;

	static RIO_EXTENSION_FUNCTION_TABLE rio;
};
} // namespace network
} // namespace core