#pragma once

#include "core_header.h"
#include "ip_endpoint.h"
#include "io_context.hpp"
#include "lock.h"
#include "net_buffer.h"
#include <queue>

namespace core {
namespace network {
class IocpSession : public std::enable_shared_from_this<IocpSession>
{
public:
	IocpSession();
	virtual ~IocpSession();
};
}// namespace network
}// namespace core