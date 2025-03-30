#pragma once

#include "net_header.h"
#include "socket_header.h"

#ifdef _WIN32
namespace core {
namespace network {
// @detail 핸들 랩핑 클래스
class IocpCore final
{
public:
	IocpCore();
	IocpCore(const IocpCore& _other) = delete;
	IocpCore(IocpCore&& _other) = delete;
	IocpCore& operator=(const IocpCore& _other) = delete;
	IocpCore& operator=(IocpCore&& _other) = delete;
	~IocpCore();

	bool Initialize();
	void Finalize();

public:
	bool Registered(HANDLE _handle, ULONG_PTR _completion_key);
	bool PostStatus(ULONG_PTR _completion_key, DWORD _transferred_bytes, OVERLAPPED* _overlapped = nullptr); 
	bool GetStatus(ULONG_PTR* _completion_key, DWORD* _transferred_bytes, OVERLAPPED** _overlapped, DWORD _timeout, OUT int& _wsa_error);

	HANDLE GetHandle() const;

private:
	bool CreateHandle();
	void CloseHandle();

private:
	HANDLE m_handle;
};
} // namespace network
} // namespace core
#endif // _WIN32