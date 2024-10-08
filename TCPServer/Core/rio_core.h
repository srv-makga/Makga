#pragma once

#include "net_header.h"
#include "socket_header.h"
#include "service.h"

#ifdef _WIN32
namespace core {
namespace network {
char* GetVirtualMemory(std::size_t _buffer_size);

enum COMPLETION_KEY
{
	CK_STOP = 0,
	CK_START = 1
};

class RIOCore : public core::server::Service
{
public:
	RIOCore();
	virtual ~RIOCore();

	bool Initialize() override;
	void Finalize() override;
	bool Start() override;
	bool Stop() override;

	ULONG DequeueCompletion(RIO_CQ& _completion_queue, std::vector<RIORESULT>& _results);
	inline static RIO_EXTENSION_FUNCTION_TABLE s_function_table = { };

private:
	bool CreateCompletionQueue(DWORD _size);

private:
	SOCKET m_socket;
	HANDLE m_iocp_handle;
	RIO_CQ m_completion_queue;
	RIO_RQ m_request_queue;
};

#define RIO RIOCore::s_function_table;
} // namespace network
} // namespace core
#endif // _WIN32