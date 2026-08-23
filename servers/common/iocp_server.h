#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "iocp_session.h"
#include "network_lifecycle_sweeper.h"

import makga.network.endpoint;
import makga.network.session;
import makga.network.iocp.service;
import makga.network.iocp.acceptor;
import makga.network.nethandler;
import makga.network.jobhandler;
import makga.lib.lock;

class IocpServer : public makga::network::IocpService, public std::enable_shared_from_this<IocpServer>
{
public:
	using NetHandler_t = makga::network::NetHandler;
	using JobHandler_t = makga::network::JobHandler;

public:
	IocpServer(makga::network::IPEndPoint ep, std::shared_ptr<makga::network::IocpCore> core);
	virtual ~IocpServer();

	bool Initialize(std::size_t max_connect_count, std::shared_ptr<NetHandler_t> net_handler, std::shared_ptr<JobHandler_t> job_handler);
		void Finalize();

	// Stop accepting new sessions while allowing existing sessions to drain.
	void BeginDrain();
	bool IsDraining() const { return !accepting_.load(std::memory_order_acquire); }
	bool IsDrainComplete() const;
	void DisconnectAllSessions();
    void SweepIoLifecycles();
	std::size_t GetAvailableSessionCount() const;

	std::shared_ptr<IocpSession> FindSession(makga::network::NetSession::Id id) const;

public: // IocpService
	makga::network::IPEndPoint GetEndPoint() const override;
	std::size_t GetConnectCount() const override;
	std::size_t GetMaxConnectCount() const override;

	std::shared_ptr<makga::network::NetSession> AllocSession() override;
	void DeallocSession(std::shared_ptr<makga::network::NetSession> session) override;
    void ScheduleSendBatch(makga::network::NetSession::Id session_id) override;

protected:
	virtual void CreateSession(std::size_t max_connect_count) = 0;
	void DestroyAllSession();
    void FlushSendBatches();

protected:
	std::shared_ptr<makga::network::IocpAcceptor> acceptor_;
	makga::network::IPEndPoint ep_;

	mutable makga::lib::SharedMutex session_mutex_;
	std::unordered_map<makga::network::NetSession::Id, std::shared_ptr<IocpSession>> sessions_;
	std::queue<std::shared_ptr<IocpSession>> free_sessions_;
	makga::network::NetSession::Id next_session_id_;

	std::shared_ptr<NetHandler_t> net_handler_;
	std::shared_ptr<JobHandler_t> job_handler_;
	std::atomic<bool> accepting_{false};
    NetworkLifecycleSweeper lifecycle_sweeper_;
    NetworkLifecycleSweeper send_batch_sweeper_{std::chrono::milliseconds(2)};
    mutable std::mutex send_batch_mutex_;
    std::queue<std::shared_ptr<IocpSession>> send_batch_queue_;
    std::unordered_set<makga::network::NetSession::Id> send_batch_session_ids_;
};
