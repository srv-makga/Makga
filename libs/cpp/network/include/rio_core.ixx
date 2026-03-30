module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

export module makga.network.rio.core;

namespace makga::network {
// Use the RIO extension table provided by the Windows headers
using RioExtensionFunctionTable = RIO_EXTENSION_FUNCTION_TABLE;

export class RioCore
{
public:
	RioCore();
	virtual ~RioCore();

	bool Initialize();
	void Finalize();

	RIO_CQ GetCompletionQueue() const;
	const RioExtensionFunctionTable& GetRioFunctionTable() const;
	HANDLE GetCompletionPort() const;

	bool PostStatus(ULONG_PTR completion_key, DWORD transferred_bytes, OVERLAPPED* overlapped = nullptr);

	RIO_BUFFERID RegisterBuffer(PCHAR buffer, DWORD buffer_length);
	void DeregisterBuffer(RIO_BUFFERID buffer_id);

	RIO_RQ CreateRequestQueue(SOCKET socket, ULONG max_outstanding_receive, ULONG max_receive_data_buffers,
		ULONG max_outstanding_send, ULONG max_send_data_buffers, RIO_CQ receive_cq, RIO_CQ send_cq, PVOID context);
	bool ResizeRequestQueue(RIO_RQ request_queue, DWORD max_outstanding_receive, DWORD max_outstanding_send);

	bool Receive(RIO_RQ request_queue, PRIO_BUF rio_buf, ULONG data_buffer_count, DWORD flags, PVOID request_context);
	bool ReceiveEx(RIO_RQ request_queue, PRIO_BUF rio_buf, ULONG data_buffer_count, PRIO_BUF local_address, PRIO_BUF remote_address, PRIO_BUF control_context, PRIO_BUF flags, DWORD operation_flags, PVOID request_context);
	bool Send(RIO_RQ request_queue, PRIO_BUF rio_buf, ULONG data_buffer_count, DWORD flags, PVOID request_context);
	bool SendEx(RIO_RQ request_queue, PRIO_BUF rio_buf, ULONG data_buffer_count, PRIO_BUF local_address, PRIO_BUF remote_address, PRIO_BUF control_context, PRIO_BUF flags, DWORD operation_flags, PVOID request_context);

	ULONG DequeueCompletion(RIO_CQ cq, PRIORESULT results, ULONG results_count);
	bool ResizeCompletionQueue(RIO_CQ cq, DWORD queue_size);
	INT Notify(RIO_CQ cq);

private:
	bool LoadRioFunctions();
	bool CreateCompletionQueue();
	void Cleanup();

private:
	RioExtensionFunctionTable rio_functions_;
	RIO_CQ completion_queue_;
	SOCKET dummy_socket_; // Used for loading extensions
	HANDLE iocp_handle_;
	bool wsa_initialized_;
};
} // namespace makga::network