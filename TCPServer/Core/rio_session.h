#pragma once

#include "core_header.h"
#include "ip_endpoint.h"
#include "io_context.hpp"
#include "lock.h"
#include "rio_event.h"
#include <queue>

class RioBuffer;
class RIOService;

namespace core {
namespace network {
class RIOSession : public std::enable_shared_from_this<RIOSession>
{
public:
	RIOSession();
	RIOSession(const RIOSession& other) = delete;
	RIOSession(RIOSession&& other) = delete;
	RIOSession& operator=(const RIOSession& other) = delete;
	RIOSession& operator=(RIOSession&& other) = delete;
	virtual ~RIOSession();

	void Initialize();
	void Finalize();

	// get
	SOCKET& GetSocket() { return m_socket; }
	SockAddress GetAddr() { return m_address; }

	// set
	void SetAddr(SOCKADDR_IN addr) { m_address = SockAddress(addr); }
	void SetAddr(SockAddress sockAddr) { m_address = sockAddr; }
	void SetCore(shared_ptr<RIOService> core) { m_RIOService = core; }

	// state check
	bool IsConnected() { return m_bConnected; }
	bool IsEmptySendQueue() { return m_sendBufQueue.empty(); }
	bool IsAllocated() { return m_bAllocated; }

	// networking
	void Disconnect();
	void Send(std::shared_ptr<NetBuffer> sendBuffer);

	// dispatch
	void Dispatch(RIOEvent* _event, int _bytes_transferred = 0);

	// DeferredSend & Commit
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
	std::shared_ptr<RIOService> m_RIOService;

	// request queue
	RIO_RQ m_requestQueue;

	// Rio Recv
	RIORecvEvent m_recvEvent;

	// Rio Send
	std::queue<std::shared_ptr<NetBuffer>> m_sendBufQueue;
	RWMutex m_sendQueueLock;
	std::atomic<int> m_sendCnt;
	time_t m_last_send_time;

	RIO_BUFFERID m_recv_buff_id;
	RIO_BUFFERID m_send_buff_id;

	std::shared_ptr<RioBuffer> m_recv_buffer;
	std::shared_ptr<RioBuffer> m_send_buffer;
};
} // namespace network
} // namespace core