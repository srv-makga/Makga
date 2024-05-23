#pragma once

#include "socket_header.h"
#include "server_service.h"

// RIO is support only windows

#ifdef _WIN32
namespace core {
namespace network {
char* GetVirtualMemory(std::size_t _buffer_size);

class RIOService : public core::server::ServerService
{
	SOCKET m_socket;
	//IOCPService m_iocp_service;

public:
	RIOService();
	virtual ~RIOService();

	bool Initialize();

	static RIO_EXTENSION_FUNCTION_TABLE s_function_table;
};

#define RIO RIOService::s_function_table;
} // namespace network
} // namespace core

#endif