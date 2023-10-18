#pragma once

#include "net_header.h"
#include <cassert>

namespace core {
namespace network {
#if defined(WIN32) 
class NetService
{
public:
	NetService()
	{
		WSADATA wsa_data;

		if (0 != ::WSAStartup(MAKEWORD(2, 2), &wsa_data))
		{
			assert(false);
		}
	}

	~NetService()
	{
		::WSACleanup();
	}
};
#endif
} // namespace network
} // namespace core