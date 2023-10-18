#pragma once

#include <cstdint>
#include <functional>

class session;

namespace core {
namespace network {
enum class net_error_code
{
	success,
	closed
};
} // namespace network

#ifndef SOCKET
using SOCKET = uintptr_t;
#endif // !SOCKET

#ifndef HANDLE
using HANDLE = void*;
#endif // !HANDLE

using thread_id_t = uint32_t;

using CallbackAccept = std::function<void(session*)>;
using CallbackConnect = std::function<void(session*, core::network::net_error_code)>;
using CallbackSend = std::function<void(session, uint32_t)>;
using CallbackRead = std::function<void(session, char*, uint32_t)>;
} // namespace core