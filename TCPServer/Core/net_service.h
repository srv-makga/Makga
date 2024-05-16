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
};
#endif
} // namespace network
} // namespace core