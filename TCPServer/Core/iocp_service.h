#pragma once

#include "net_header.h"
#include "socket_header.h"
#include "server_service.h"

namespace core {
namespace network {
class IOCPService : public server::ServerService
{
public:
	IOCPService();
	virtual ~IOCPService();

	void Initialize();
	void Finalize();
	bool CreatePort(DWORD _concurrent_thread = 1);
	bool Registered(HANDLE _handle, ULONG_PTR _completion_key);
	bool PostStatus(ULONG_PTR _completion_key, DWORD _transferred_bytes, OVERLAPPED* _overlapped = nullptr);
	bool GetStatus(ULONG_PTR* _completion_key, DWORD* _transferred_bytes, OVERLAPPED** _overlapped, DWORD _timeout = INFINITE);

	bool Start() override;
	bool Stop() override;

private:
	HANDLE m_iocp;
};
} // namespace network
} // namespace core