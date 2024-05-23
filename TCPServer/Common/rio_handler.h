#pragma once

#include "common_header.h"
#include "net_handler.h"

#include "../Core/thread_group.h"
#include "../Core/thread.h"
#include "../Core/rio_service.h"
#include "../Core/io_context.hpp"

class RIOHandler : public NetHandler
{
public:
	using Handler_t = core::network::RIOService;
	using Thread_t = core::thread::ThreadGroup;

public:
	RIOHandler();
	virtual ~RIOHandler();

	RIOHandler(const RIOHandler&) = delete;
	RIOHandler(RIOHandler&&) = delete;

	RIOHandler& operator=(const RIOHandler&) = delete;
	RIOHandler& operator=(RIOHandler&&) = delete;

public:
	bool Initialize(std::size_t) override;
	bool Finallize() override;
	bool RegistedHandle(HANDLE, ULONG_PTR) override;
	bool Stop() override;

	bool PostEvent(ULONG_PTR _key, IOContext_t* _io_context);

private:
	void Run();

private:
	Handler_t m_service;
	Thread_t m_thread_group;

	std::time_t m_time_out;
	std::size_t m_thread_count;

	static RIO_EXTENSION_FUNCTION_TABLE s_rio_table;

};