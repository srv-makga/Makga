#pragma once

#include "session.h"
#include "ip_endpoint.h"
#include "rio_core.h"
#include "rio_service.h"
#include "rio_event.h"
#include "rio_buffer.h"
#include "lock.h"
#include "net_buffer.h"
#include <queue>

namespace core {
namespace network {
class RioSession : public Session, public std::enable_shared_from_this<RioSession>
{
public:
	RioSession(Id _id, Session::Type _type);
	RioSession(const RioSession& _other) = delete;
	RioSession(RioSession&& _other) = delete;
	RioSession& operator=(const RioSession& _other) = delete;
	RioSession& operator=(RioSession&& _other) = delete;
	virtual ~RioSession();

public:
	bool Initialize() override;
	void Finalize() override;

	Id GetSessionId() const override;
	void SetSessionId(Id _id) override;

	Type GetSessionType() const override;
	void SetSessionType(Type _type) override;

	void Disconnect();
	void Send(std::shared_ptr<NetBuffer> _buffer);

	void Dispatch(RioEvent* _event, int _bytes_transferred = 0);

	bool SendDeferred();
	void SendCommit();

	void RegisterRecv();
	bool RegisterSend(ULONG dataLength, ULONG dataOffset);

	void ProcessConnect();
	void ProcessRecv(std::size_t _bytes_transferred);
	void ProcessSend(std::size_t _bytes_transferred, RioSendEvent* _event);

public:
	virtual void OnConnected() {}
	virtual std::size_t OnRecv(char* _buffer, std::size_t _length) { return 0; }
	virtual void OnSend(std::size_t _length) {}
	virtual void OnDisconnected() {}

public:
	SOCKET GetSocket();
	void SetSocket(SOCKET _socket);

	const IPEndPoint& GetEndPoint() const;
	const sockaddr_in GetSockAddr() const;
	void SetSockAddr(sockaddr_in _sockaddr);

	std::shared_ptr<RioCore> GetCore(std::shared_ptr<RioCore> _core) const;
	void SetCore(std::shared_ptr<RioCore> _core);

	bool IsConnected() const;
	bool IsEmptySendQueue() const;
	bool IsAllocated() const;

private:
	bool AllocBuffer();
	bool CreateRequestQueue();

private:
	Id m_id;
	Type m_type;

	SOCKET m_socket;
	IPEndPoint m_end_point;

	std::atomic<bool> m_is_connected;
	std::atomic<bool> m_is_allocated;

	std::shared_ptr<RioCore> m_rio_core;

	RIO_RQ m_rio_request_queue;

	RioRecvEvent m_recv_event;

	std::queue<std::shared_ptr<NetBuffer>> m_send_buffer_queue;
	mutable RWMutex m_mutex_send_queue;
	std::atomic<std::size_t> m_send_count;
	time_t m_last_send_time;

	RIO_BUFFERID m_recv_buff_id;
	RIO_BUFFERID m_send_buff_id;

	std::shared_ptr<RioBuffer> m_recv_buffer;
	std::shared_ptr<RioBuffer> m_send_buffer;
};
} // namespace network
} // namespace core