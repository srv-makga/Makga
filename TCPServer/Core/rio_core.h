#pragma once

#include "net_header.h"
#include "socket_header.h"
#include "service.h"
#include "rio_session.h"

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
	using Session_t = std::shared_ptr<RIOSession>;

public:
	RIOCore();
	RIOCore(const RIOCore& _other) = delete;
	RIOCore(RIOCore&& _other) = delete;
	RIOCore& operator=(const RIOCore& _other) = delete;
	RIOCore& operator=(RIOCore&& _other) = delete;
	virtual ~RIOCore();

	bool Initialize() override;
	void Finalize() override;
	bool Start() override;
	bool Stop() override;

public: // RIO
	bool Dispatch();
	void DeferredSend();
	ULONG DequeueCompletion(RIO_CQ& _completion_queue, std::vector<RIORESULT>& _results);

	RIO_CQ& CompletionQueue() { return m_completion_queue; }

protected:
	bool CreateCompletionQueue(DWORD _size);

protected:
	SOCKET m_socket;
	HANDLE m_iocp_handle;
	RIO_CQ m_completion_queue;
	RIORESULT m_results[MAX_RIO_RESULT];

public:
	inline static RIO_EXTENSION_FUNCTION_TABLE s_function_table = { };
};

#define RIO RIOCore::s_function_table;
} // namespace network
} // namespace core
#endif // _WIN32