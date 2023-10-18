#pragma once

#include "net_header.h"

namespace core {
namespace network {
class IOCPService
{
	HANDLE	m_iocp;

public:
	IOCPService();
	~IOCPService();

	void Initialize();
	void Finalize();
	bool CreatePort(DWORD _concurrent_thread = 1);
	bool Registered(HANDLE _handle, ULONG_PTR _completion_key);
	bool PostStatus(ULONG_PTR _completion_key, DWORD _transferred_bytes, OVERLAPPED* _overlapped = nullptr);
	bool GetStatus(ULONG_PTR* _completion_key, DWORD* _transferred_bytes, OVERLAPPED** _overlapped, DWORD _timeout = INFINITE);
};
} // namespace network
} // namespace core