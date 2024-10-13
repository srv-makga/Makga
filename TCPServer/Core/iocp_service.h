#pragma once

#include "net_header.h"
#include "socket_header.h"
#include "service.h"

class IOCPSession;

#ifdef _WIN32
namespace core {
namespace network {
class IOCPService : public server::Service
{
public:
	IOCPService();
	IOCPService(const IOCPService& _other) = delete;
	IOCPService(IOCPService&& _other) = delete;
	IOCPService& operator=(const IOCPService& _other) = delete;
	IOCPService& operator=(IOCPService&& _other) = delete;
	virtual ~IOCPService();

	bool Initialize() override;
	void Finalize() override;
	bool Start() override;
	bool Stop() override;

public: // iocp
	bool CreatePort(DWORD _concurrent_thread = 1);
	bool Registered(HANDLE _handle, ULONG_PTR _completion_key);
	bool PostStatus(ULONG_PTR _completion_key, DWORD _transferred_bytes, OVERLAPPED* _overlapped = nullptr); 
	bool GetStatus(ULONG_PTR* _completion_key, DWORD* _transferred_bytes, OVERLAPPED** _overlapped, DWORD _timeout = INFINITE);

private:
	HANDLE m_iocp;
	unsigned int m_thread_count;
};
} // namespace network
} // namespace core
#endif // _WIN32