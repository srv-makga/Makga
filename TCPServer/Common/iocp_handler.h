#pragma once

#include "common_header.h"
#include "net_handler.h"

#include "../Core/thread_group.h"
#include "../Core/thread.h"
#include "../Core/iocp_service.h"
#include "../Core/io_context.hpp"

class IOCPHandler : public NetHandler
{
public:
	using Service_t = core::network::IOCPService;
	using Thread_t = core::thread::ThreadGroup;

public:
	IOCPHandler();
	virtual ~IOCPHandler();

	IOCPHandler(const IOCPHandler&) = delete;
	IOCPHandler(IOCPHandler&&) = delete;

	IOCPHandler& operator=(const IOCPHandler&) = delete;
	IOCPHandler& operator=(IOCPHandler&&) = delete;

public:
	bool Initialize() override;
	bool Finallize() override;
	bool RegistedHandle(HANDLE, ULONG_PTR) override;
	bool Start(std::size_t _thread_count) override;
	bool Stop() override;

	bool PostEvent(ULONG_PTR _key, IOContext_t* _io_context);

private:
	void Run();

private:
	Service_t m_service;
	Thread_t m_thread_group;

	std::time_t m_time_out;
	std::size_t m_thread_count;
};