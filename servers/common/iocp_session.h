#pragma once

//#include <WinSock2.h>
//#include <windows.h>
//#include <memory>
#include "net_packet.h"

import <queue>;
import makga.lib.lock;
import makga.network.session;
import makga.network.endpoint;
import makga.network.iocp.acceptor;
import makga.network.iocp.event;
import makga.network.iocp.object;
import makga.network.iocp.service;

class IocpSession : public makga::network::IocpObject, public makga::network::NetSession
{
	friend class IocpAcceptor;

public:
	IocpSession(std::shared_ptr<makga::network::IocpService> service);
	virtual ~IocpSession();

	bool Initialize();
	void Finalize();

	bool Connect();
	void Disconnect();
	void Send(std::shared_ptr<NetPacket> send_buffer);

	// @brief 동기 요청
	int Recv();

	// @brief 비동기 요청
	bool RegisterConnect();
	void RegisterDisconnect();
	void RegisterZeroRecv();
	void RegisterRecv();
	void RegisterSend();

	std::shared_ptr<makga::network::IocpService> GetService() const;

	bool IsConnected() const;
	bool IsSendRegistered() const;
	bool IsZeroReceive() const;

public: // NetEvent의 처리 함수들(단일 데이터)
	virtual void ProcConnect() = 0;
	virtual void ProcDisconnect() = 0;
	virtual std::size_t ProcRecv(char* data, std::size_t bytes_transferred) = 0;
	virtual void ProcSend(int32_t bytes_transferred) = 0;

public: // IocpObject
	void Dispatch(makga::network::IocpEvent* event, int32_t bytes_transferred = 0) override;
	HANDLE GetHandle() const override;

public: // NetSession
	void OnConnect() override;
	void OnDisconnect() override;
	void OnRecv(int bytes_transferred) override;
	void OnSend(int bytes_transferred) override;

	SOCKET GetSocket() const override;
	char* GetWritePosition() override;

protected:
	SOCKET socket_;

	std::atomic<bool> is_connected_;
	bool is_zero_receivce_;

	std::atomic<std::time_t> recv_time_;
	std::atomic<std::size_t> recv_size_;

	std::atomic<std::time_t> sent_time_;
	std::atomic<std::size_t> sent_size_;

	std::shared_ptr<NetPacket> recv_buffer_;

	makga::lib::Mutex send_mutex_;
	std::queue<std::shared_ptr<NetPacket>> send_buffer_queue_;
	std::atomic<bool> is_send_registered_;

	makga::network::IocpRecvEvent recv_event_;
	makga::network::IocpConnectEvent connect_event_;
	makga::network::IocpDisconnectEvent disconnect_event_;
	makga::network::IocpSendEvent<NetPacket> send_event_;

	std::shared_ptr<makga::network::IocpService> service_;
};