module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

module makga.network.iocp.core;

namespace makga::network {
IocpCore::IocpCore()
	: handle_(INVALID_HANDLE_VALUE)
{
}

IocpCore::~IocpCore()
{
	Finalize();
}

bool IocpCore::Initialize()
{
	if (handle_ != INVALID_HANDLE_VALUE && handle_ != NULL)
		return true;
	return CreateHandle();
}

void IocpCore::Finalize()
{
	CloseHandle();
}

bool IocpCore::Registered(HANDLE handle, ULONG_PTR completion_key)
{
	if (handle_ == INVALID_HANDLE_VALUE || handle_ == NULL || handle == INVALID_HANDLE_VALUE || handle == NULL)
		return false;
	return handle_ == ::CreateIoCompletionPort(handle, handle_, completion_key, 0);
}

bool IocpCore::PostStatus(ULONG_PTR completion_key, DWORD transferred_bytes, OVERLAPPED* overlapped)
{
	return handle_ != INVALID_HANDLE_VALUE && handle_ != NULL &&
		TRUE == ::PostQueuedCompletionStatus(handle_, transferred_bytes, completion_key, overlapped);
}

bool IocpCore::GetStatus(ULONG_PTR* completion_key, OUT DWORD& transferred_bytes, OVERLAPPED** overlapped, DWORD timeout, OUT int& wsa_error)
{
	wsa_error = ERROR_SUCCESS;
	if (handle_ == INVALID_HANDLE_VALUE || handle_ == NULL || completion_key == nullptr || overlapped == nullptr)
	{
		wsa_error = ERROR_INVALID_HANDLE;
		return false;
	}
	if (TRUE == ::GetQueuedCompletionStatus(handle_, &transferred_bytes, completion_key, overlapped, timeout))
		return true;

	wsa_error = static_cast<int>(::GetLastError());
	// A failed asynchronous operation still has an OVERLAPPED completion. Dispatch it so
	// the owner can decrement its pending-I/O count and perform protocol-specific cleanup.
	return *overlapped != nullptr;
}

HANDLE IocpCore::GetHandle() const
{
	return handle_;
}

bool IocpCore::CreateHandle()
{
	handle_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	return handle_ != NULL;
}

void IocpCore::CloseHandle()
{
	if (handle_ != INVALID_HANDLE_VALUE && handle_ != NULL)
		::CloseHandle(handle_);
	handle_ = INVALID_HANDLE_VALUE;
}
}// namespace makga::network
