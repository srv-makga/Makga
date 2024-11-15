#pragma once

#include "net_header.h"
#include "socket_header.h"
#include "service.h"
#include "rio_session.h"

class RIOSession;

#ifdef _WIN32
namespace core {
namespace network {
char* GetVirtualMemory(std::size_t _buffer_size);

enum COMPLETION_KEY
{
	CK_STOP = 0,
	CK_START = 1
};

class RIOService : public core::server::Service
{
public:
	using Session_t = std::shared_ptr<RIOSession>;

public:
	RIOService();
	RIOService(const RIOService& _other) = delete;
	RIOService(RIOService&& _other) = delete;
	RIOService& operator=(const RIOService& _other) = delete;
	RIOService& operator=(RIOService&& _other) = delete;
	virtual ~RIOService();

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

#define RIO RIOService::s_function_table;
} // namespace network
} // namespace core
#endif // _WIN32