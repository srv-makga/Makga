module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif


export module makga.network.iocp.core;

namespace makga::network {
export class IocpCore
{
public:
	IocpCore();
	virtual ~IocpCore();

	bool Initialize();
	void Finalize();

	bool Registered(HANDLE handle, ULONG_PTR completion_key);
	bool PostStatus(ULONG_PTR completion_key, DWORD transferred_bytes, OVERLAPPED* overlapped = nullptr);
	bool GetStatus(ULONG_PTR* completion_key, OUT DWORD& transferred_bytes, OVERLAPPED** overlapped, DWORD timeout, OUT int& wsa_error);

	HANDLE GetHandle() const;

private:
	bool CreateHandle();
	void CloseHandle();

private:
	HANDLE handle_;
};
} // namespace makga::network