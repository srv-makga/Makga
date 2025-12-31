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

import makga.network.endpoint;

export namespace makga::network {
export class Socket
{
public:
	Socket()
		: socket_(INVALID_SOCKET)
	{
	}

	virtual ~Socket()
	{
		Close();
	}

	void Close()
	{
		if (INVALID_SOCKET != socket_)
		{
			::shutdown(socket_, SD_BOTH);
			::closesocket(socket_);
			socket_ = INVALID_SOCKET;
		}
	}

	SOCKET GetSocket() const
	{
		return socket_;
	}

	operator SOCKET() const
	{
		return socket_;
	}

	operator bool() const
	{
		return INVALID_SOCKET != socket_;
	}

	operator HANDLE() const
	{
		return reinterpret_cast<HANDLE>(socket_);
	}

	SOCKET operator= (SOCKET socket)
	{
		socket_ = socket;
		return socket_;
	}

protected:
	SOCKET socket_;
};
} // namesapce makga::network