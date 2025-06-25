#pragma once

#include "net_header.h"
#include "net_core.h"
#include "socket_header.h"

#ifdef _WIN32
namespace core {
namespace network {
// @detail 핸들 랩핑 클래스
class IocpCore final : public NetCore
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

public: // NetCore
	CoreType GetCoreType() const override { return CoreType::IOCP; }

public:
	bool Registered(HANDLE _handle, ULONG_PTR _completion_key);
	bool PostStatus(ULONG_PTR _completion_key, DWORD _transferred_bytes, OVERLAPPED* _overlapped = nullptr); 
	bool GetStatus(ULONG_PTR* _completion_key, OUT DWORD& _transferred_bytes, OVERLAPPED** _overlapped, DWORD _timeout, OUT int& _wsa_error);

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