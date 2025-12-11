module;

#include <windows.h>
#include <memory>
#include <mutex>

export module makga.network.iocp.session;

import <queue>;
import makga.network.endpoint;
import makga.network.buffer;
import makga.network.iocp.acceptor;
import makga.network.iocp.event;
import makga.network.iocp.object;
import makga.network.iocp.service;

export namespace makga::network {
export class IocpSession : public IocpObject
{
friend class IocpAcceptor;
public:
	using Id = uint64_t;

	IocpSession(std::shared_ptr<IocpService> service);
	virtual ~IocpSession();

	bool Initialize();
	void Finalize();

	bool Connect();
	void Disconnect();
	void Send(std::shared_ptr<NetBuffer> send_buffer);

	// @brief 동기 요청
	int Recv();

	// @brief 비동기 요청
	bool RegisterConnect();
	void RegisterDisconnect();
	void RegisterZeroRecv();
	void RegisterRecv();
	void RegisterSend();

	// @brief 비동기 요청의 결과로 호출
	void OnConnect();
	void OnDisconnect();
	void OnRecv(int bytes_transferred);
	void OnSend(int bytes_transferred);

	Id GetSessionId() const;
	void SetSessionId(Id id);

	SOCKET GetSocket() const;

	const IPEndPoint& GetEndPoint() const;
	void SetEndPoint(SOCKADDR_IN addr);
	void SetEndPoint(IPEndPoint ep);

	std::shared_ptr<IocpService> GetService() const;

	bool IsConnected() const;
	bool IsSendRegistered() const;
	bool IsZeroReceive() const;

public: // NetEvent의 처리 함수들(단일 데이터)
	virtual void ProcConnect() = 0;
	virtual void ProcDisconnect() = 0;
	virtual std::size_t ProcRecv(char* data, int32_t bytes_transferred) = 0;
	virtual void ProcSend(std::int32_t bytes_transferred) = 0;

public: // IocpObject
	void Dispatch(IocpEvent* event, int32_t bytes_transferred = 0) override;
	HANDLE GetHandle() const override;

protected:
	Id id_;

	SOCKET socket_;
	IPEndPoint ep_;

	std::atomic<bool> is_connected_;
	bool is_zero_receivce_;

	std::atomic<std::time_t> recv_time_;
	std::atomic<std::size_t> recv_size_;

	std::atomic<std::time_t> sent_time_;
	std::atomic<std::size_t> sent_size_;

	std::shared_ptr<NetBuffer> recv_buffer_;

	std::mutex send_mutex_;
	std::queue<std::shared_ptr<NetBuffer>> send_buffer_queue_;
	std::atomic<bool> is_send_registered_;

	IocpRecvEvent recv_event_;
	IocpConnectEvent connect_event_;
	IocpDisconnectEvent disconnect_event_;
	IocpSendEvent<NetBuffer> send_event_;

	std::shared_ptr<IocpService> service_;
};
} // namespace makga::network