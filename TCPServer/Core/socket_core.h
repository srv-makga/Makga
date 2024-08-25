#pragma once

#include "net_header.h"
#include "singleton.hpp"

namespace core {
namespace network {
class SocketCore
{
public:
	SocketCore() = default;
	virtual ~SocketCore() = default;

	static void Initialize();

	inline static LPFN_ACCEPTEX fn_acceptex = nullptr;
	inline static LPFN_GETACCEPTEXSOCKADDRS fn_acceptex_sock_addr = nullptr;
	inline static LPFN_CONNECTEX fn_connectex = nullptr;
	inline static LPFN_DISCONNECTEX fn_disconnectex = nullptr;
};
} // namespace network
} // namespace core