#pragma once

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <memory>
#include <queue>

#include "net_packet.h"

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
    enum class ShutdownMode : std::uint8_t { Active, Graceful, Forced };
    bool RequestGracefulClose(std::chrono::milliseconds timeout = std::chrono::seconds(10));
    void ForceClose();
    ShutdownMode GetShutdownMode() const { return shutdown_mode_.load(std::memory_order_acquire); }
    void RequestLifecycleClose();
    bool SweepIoLifecycle();
    bool IsIoLifecycleSafe() const;
	static constexpr std::size_t kInitialReceiveBufferBytes = 8 * 1024;
    static constexpr std::size_t kMaxReceiveBufferBytes = 8 * 1024 * 1024 + sizeof(NetPacket::Header);
	static constexpr std::size_t kSoftPendingSendBytes = 4 * 1024 * 1024;
    static constexpr std::size_t kHardPendingSendBytes = 16 * 1024 * 1024;
    static constexpr std::size_t kMaxPendingSendBytes = 32 * 1024 * 1024;
	static constexpr std::size_t kMaxBuffersPerSend = 64;
    static constexpr std::size_t kBatchFlushBytes = 32 * 1024;
    static constexpr std::uint64_t kBatchFlushDelayMs = 2;

    enum class SendPriority : std::uint8_t { Low, Normal, Critical };
    void Send(std::shared_ptr<NetPacket> send_buffer);
    bool SendWithPriority(std::shared_ptr<NetPacket> send_buffer, SendPriority priority);
    // Called only by the dedicated IOCP batch scheduler, never by gameplay workers.
    bool FlushSendBatch(std::uint64_t now_tick);
    bool HasQueuedSendBatch() const { return queued_send_bytes_.load(std::memory_order_acquire) != 0; }
	std::size_t GetPendingSendBytes() const { return pending_send_bytes_.load(std::memory_order_acquire); }

	int Recv();

	bool RegisterConnect();
	void RegisterDisconnect();
	void RegisterZeroRecv();
	void RegisterRecv();
	void RegisterSend();

	std::shared_ptr<makga::network::IocpService> GetService() const;

	bool IsConnected() const;
	bool IsSendRegistered() const;
	bool IsZeroReceive() const;

public: // NetEvent
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
    // Compacts consumed bytes then grows only up to the per-session hard bound.
    bool EnsureReceiveCapacity(std::size_t required_write_bytes);

	void ReleasePendingSendBytes(std::size_t bytes)
	{
		std::size_t current = pending_send_bytes_.load(std::memory_order_acquire);
		while (current != 0)
		{
			const std::size_t next = bytes >= current ? 0 : current - bytes;
			if (pending_send_bytes_.compare_exchange_weak(current, next, std::memory_order_acq_rel))
				return;
		}
	}

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
	std::atomic<std::size_t> pending_send_bytes_{0};
    std::atomic<std::size_t> queued_send_bytes_{0};
    std::atomic<std::uint64_t> next_batch_flush_tick_{0};
    std::atomic_bool zero_recv_posted_{false};
    std::atomic<ShutdownMode> shutdown_mode_{ShutdownMode::Active};
    std::atomic<std::uint64_t> graceful_deadline_tick_{0};
    std::atomic<bool> disconnect_posted_{false};

	makga::network::IocpRecvEvent recv_event_;
	makga::network::IocpConnectEvent connect_event_;
	makga::network::IocpDisconnectEvent disconnect_event_;
	makga::network::IocpSendEvent<NetPacket> send_event_;

	std::shared_ptr<makga::network::IocpService> service_;
};
