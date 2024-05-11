#pragma once

#include "net_header.h"
#include <cassert>

namespace core {
namespace network {
#if defined(WIN32) 
class NetService
{
public:
	NetService();
	~NetService();

public:
	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* func);

	//LPFN_CONNECTEX		ConnectEx;
	//LPFN_DISCONNECTEX	DisconnectEx;
	//LPFN_ACCEPTEX		AcceptEx;

};
#endif
} // namespace network
} // namespace core