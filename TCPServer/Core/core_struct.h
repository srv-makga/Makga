#pragma once

#include "core_type.h"
#include "net_header.h"

namespace core {
#if defined(_WIN32)
struct SmartHandle
{
	HANDLE handle;

	SmartHandle() : handle(INVALID_HANDLE_VALUE) {}
	~SmartHandle() { if (INVALID_HANDLE_VALUE != handle) ::CloseHandle(handle); }

	operator HANDLE() const { return handle; }
};
#endif

struct SmartSocket
{
	Socket_t socket;

	SmartSocket() : socket(INVALID_SOCKET) {}
	~SmartSocket() { if (INVALID_SOCKET != socket) ::closesocket(socket); }

	operator Socket_t() const { return socket; }
};
} // namespace core