#pragma once

#include "core_header.h"

namespace core {
namespace queue {
template<typename T>
concept iocp_queue_object_type = std::is_pointer_v<T>;

template<iocp_queue_object_type T>
class QueueIOCP
{
public:
	QueueIOCP()
		: m_iocp(INVALID_HANDLE_VALUE)
	{
	}

	~QueueIOCP()
	{
		Destroy();
	}

	bool Initialize()
	{
		return Create();
	}

	void Finalize()
	{
		Destroy();
	}

	BOOL Push(T _object)
	{
		return ::PostQueuedCompletionStatus(m_iocp, 0, (ULONG_PTR)_object, nullptr);
	}

	T Pop()
	{
		DWORD temp = 0;
		T data = nullptr;

		::GetQueuedCompletionStatus(m_iocp, &temp, (PULONG_PTR)(&data), nullptr, INFINITE);
		return data;
	}

	BOOL Pop(LPDWORD _out_bytes_transferred, PULONG_PTR _comple_key, LPOVERLAPPED* _overlapped, DWORD* _out_error)
	{
		BOOL ret = ::GetQueuedCompletionStatus(m_iocp, _out_bytes_transferred, _comple_key, _overlapped, INFINITE);

		if (FALSE == ret && nullptr != _overlapped)
		{
			*_out_error = ::GetLastError();
			return FALSE;
		}

		return ret;
	}

	bool Registered(HANDLE _handle, ULONG_PTR _key)
	{
		if (INVALID_HANDLE_VALUE == _handle)
		{
			return false;
		}

		return ::CreateIoCompletionPort(_handle, m_iocp, _key, 0) == m_iocp;
	}

	operator HANDLE() const
	{
		return m_iocp;
	}

private:
	bool Create()
	{
		m_iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1);
		return INVALID_HANDLE_VALUE != m_iocp;
	}

	void Destroy()
	{
		if (INVALID_HANDLE_VALUE != m_iocp)
		{
			::CloseHandle(m_iocp);
			m_iocp = INVALID_HANDLE_VALUE;
		}
	}

private:
	HANDLE m_iocp;
};
} // namespace queue
} // namespace core