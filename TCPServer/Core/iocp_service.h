#pragma once

#include "net_header.h"
#include "socket_header.h"
#include "service.h"

#ifdef _WIN32
namespace core {
namespace network {
class IOCPService : public server::Service
{
public:
	IOCPService();
	virtual ~IOCPService();

	bool Initialize() override;
	void Finalize() override;
	bool Start() override;
	bool Stop() override;

public:
	bool CreatePort(DWORD _concurrent_thread = 1);
	bool Registered(HANDLE _handle, ULONG_PTR _completion_key);
	bool PostStatus(ULONG_PTR _completion_key, DWORD _transferred_bytes, OVERLAPPED* _overlapped = nullptr); 
	bool GetStatus(ULONG_PTR* _completion_key, DWORD* _transferred_bytes, OVERLAPPED** _overlapped, DWORD _timeout = INFINITE);

private:
	HANDLE m_iocp;
};
} // namespace network
} // namespace core
#endif // _WIN32