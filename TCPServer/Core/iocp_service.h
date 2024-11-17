#pragma once

#include "net_header.h"
#include "socket_header.h"
#include "service.h"

class IOCPSession;

#ifdef _WIN32
namespace core {
namespace network {
class IocpService : public server::Service
{
public:
	IocpService();
	IocpService(const IocpService& _other) = delete;
	IocpService(IocpService&& _other) = delete;
	IocpService& operator=(const IocpService& _other) = delete;
	IocpService& operator=(IocpService&& _other) = delete;
	virtual ~IocpService();

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