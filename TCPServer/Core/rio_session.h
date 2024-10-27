#pragma once

#include "core_header.h"
#include "ip_endpoint.h"
#include "io_context.hpp"
#include "lock.h"
#include "rio_core.h"
#include "rio_service.h"
#include "rio_event.h"
#include "rio_buffer.h"
#include "net_buffer.h"
#include <queue>

namespace core {
namespace network {
class RioSession : public std::enable_shared_from_this<RioSession>
{
public:
	RioSession();
	RioSession(const RioSession& other) = delete;
	RioSession(RioSession&& other) = delete;
	RioSession& operator=(const RioSession& other) = delete;
	RioSession& operator=(RioSession&& other) = delete;
	virtual ~RioSession();

	void Initialize();
	void Finalize();

	SOCKET GetSocket();
	void SetSocket(SOCKET _socket);

	const IPEndPoint& GetEndPoint() const;
	const sockaddr_in GetSockAddr() const;
	void SetSockAddr(sockaddr_in _sockaddr);


	/* ���� ���� */

	void SetCore(std::shared_ptr<RIOCore> _core) { m_rio_core = _core; }

	bool IsConnected() { return m_is_connected; }
	bool IsEmptySendQueue() { return m_send_buffer_queue.empty(); }

	void Disconnect();
	void Send(std::shared_ptr<NetBuffer> _buffer);

	void Dispatch(RIOEvent* _event, int _bytes_transferred = 0);

	bool SendDeferred();
	void SendCommit();

public:
	void RegisterRecv();
	bool RegisterSend(int dataLength, int dataOffset);

	void ProcessConnect();
	void ProcessRecv(int _bytes_transferred);
	void ProcessSend(int _bytes_transferred, RIOSendEvent* _event);

public:
	virtual void OnConnected() {}
	virtual int OnRecv(char* buffer, int len) final;
	virtual void OnRecvPacket(char* buffer, int len) {}
	virtual void OnSend(int len) {}
	virtual void OnDisconnected() {}

private:
	bool InitSession();
	bool AllocBuffer();
	bool CreateRequestQueue();

private:
	SOCKET m_socket;
	IPEndPoint m_address;
	std::atomic<bool> m_is_connected;
	std::atomic<bool> m_is_allocated;

	// rio core
	std::shared_ptr<RIOCore> m_rio_core;

	// request queue
	RIO_RQ m_rio_request_queue;

	// Rio Recv
	RIORecvEvent m_recv_event;

	// Rio Send
	std::queue<std::shared_ptr<NetBuffer>> m_send_buffer_queue;
	RWMutex m_mutex_send_queue;
	std::atomic<std::size_t> m_send_count;
	time_t m_last_send_time;

	RIO_BUFFERID m_recv_buff_id;
	RIO_BUFFERID m_send_buff_id;

	std::shared_ptr<RioBuffer> m_recv_buffer;
	std::shared_ptr<RioBuffer> m_send_buffer;
};
} // namespace network
} // namespace core