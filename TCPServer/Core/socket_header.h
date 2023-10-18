#pragma once

#include "core_header.h"

namespace core {
namespace network {
enum Shutdown_e
{
	Receive = OS_DEF(SHUT_RD),
	Send = OS_DEF(SHUT_WR),
	Both = OS_DEF(SHUT_RDWR)
};

typedef int message_flags;

static constexpr int message_peek = OS_DEF(MSG_PEEK);
static constexpr int message_out_of_band = OS_DEF(MSG_OOB);
static constexpr int message_do_not_route = OS_DEF(MSG_DONTROUTE);

static constexpr int max_listen_connections = OS_DEF(SOMAXCONN);
static constexpr int max_connections = OS_DEF(SOMAXCONN);

enum Wait_t
{
	Read,
	Write,
	Error
};
} // namespace network
} // namespace core