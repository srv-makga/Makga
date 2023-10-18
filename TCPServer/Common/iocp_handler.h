#pragma once

#include "common_header.h"
#include "net_handler.h"

#include "../Core/thread_group.h"
#include "../Core/thread.h"
#include "../Core/iocp_service.h"
#include "../Core/io_context.hpp"

class JobHandler;
class NetActor;

class IOCPHandler : public NetHandler
{
public:
	using IOCP_t = core::network::IOCPService;
	using Thread_t = core::thread::ThreadGroup;

public:
	IOCPHandler();
	virtual ~IOCPHandler();

	IOCPHandler(const IOCPHandler&) = delete;
	IOCPHandler(IOCPHandler&&) = delete;

	IOCPHandler& operator=(const IOCPHandler&) = delete;
	IOCPHandler& operator=(IOCPHandler&&) = delete;

public:
	bool Initialize(std::size_t) override;
	bool Finallize() override;
	bool RegistedHandle(HANDLE, ULONG_PTR) override;
	bool Stop() override;

	bool PostEvent(ULONG_PTR _key, IOContext_t* _io_context);

private:
	void Run();

private:
	IOCP_t m_iocp_service;
	Thread_t m_thread_group;

	std::time_t m_time_out;
	std::size_t m_thread_count;
};