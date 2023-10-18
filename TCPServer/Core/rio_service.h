#pragma once

#include "socket_header.h"

// RIO is support only windows

#ifdef _WIN32
namespace core {
char* GetVirtualMemory(std::size_t _buffer_size);

class RioService final
{
	SOCKET m_socket;
	//IOCPService m_iocp_service;

public:
	RioService();
	~RioService();

	bool Initialize();

	static RIO_EXTENSION_FUNCTION_TABLE s_function_table;
	static RIO_CQ s_completion_queue;
};

#define RIO RioService::s_function_table;
} // namespace core

#endif