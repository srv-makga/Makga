module network.service.iocp.core;

#include <Windows.h>

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
	CloseHandle();
	return CreateHandle();
}

void IocpCore::Finalize()
{
	CloseHandle();
}

bool IocpCore::Registered(HANDLE handle, ULONG_PTR completion_key)
{
	return handle_ == ::CreateIoCompletionPort(handle, handle_, completion_key, 0);
}

bool IocpCore::PostStatus(ULONG_PTR completion_key, DWORD transferred_bytes, OVERLAPPED* overlapped)
{
	return TRUE == ::PostQueuedCompletionStatus(handle_, transferred_bytes, completion_key, overlapped);
}

bool IocpCore::GetStatus(ULONG_PTR* completion_key, OUT DWORD& transferred_bytes, OVERLAPPED** overlapped, DWORD timeout, OUT int& wsa_error)
{
	if (TRUE == ::GetQueuedCompletionStatus(handle_, &transferred_bytes, completion_key, overlapped, timeout))
	{
		return true;
	}

	wsa_error = ::WSAGetLastError();

	switch (wsa_error)
	{
	case WAIT_TIMEOUT:
	case ERROR_NETNAME_DELETED:
	case ERROR_OPERATION_ABORTED:
		// The I/O operation has been aborted because of either a thread exit or an application request.
	case ERROR_SEM_TIMEOUT:
		// The semaphore timeout period has expired.
		break;
	default:
		return true;
	}

	return false;
}

HANDLE IocpCore::GetHandle() const
{
	return handle_;
}

bool IocpCore::CreateHandle()
{
	handle_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	return INVALID_HANDLE_VALUE != handle_;
}

void IocpCore::CloseHandle()
{
	if (INVALID_HANDLE_VALUE != handle_)
	{
		::CloseHandle(handle_);
		handle_ = INVALID_HANDLE_VALUE;
	}
}
}// namespace makga::network