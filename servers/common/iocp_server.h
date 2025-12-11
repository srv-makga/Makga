#pragma once

#include <shared_mutex>

import makga.network.endpoint;
import makga.network.iocp.service;
import makga.network.iocp.acceptor;
import makga.network.iocp.session;
import makga.network.nethandler;
import makga.network.jobhandler;

class IocpServer : public makga::network::IocpService, public std::enable_shared_from_this<IocpServer>
{
public:
	using Session_t = makga::network::IocpSession;

public:
	IocpServer(std::shared_ptr<makga::network::IocpCore> _core);
	virtual ~IocpServer();

	bool Initialize(std::size_t max_connect_count);
	void Finalize();

	std::shared_ptr<Session_t> FindSession(Session_t::Id id) const;

public: // IocpService
	makga::network::IPEndPoint GetEndPoint() const override;
	std::size_t GetConnectCount() const override;
	std::size_t GetMaxConnectCount() const override;

	std::shared_ptr<Session_t> AllocSession() override;
	void DeallocSession(std::shared_ptr<Session_t> session) override;

private:
	void CreateSession(std::size_t max_connect_count);

protected:
	std::shared_ptr<makga::network::IocpAcceptor> acceptor_;
	makga::network::IPEndPoint ep_;

	mutable std::shared_mutex session_mutex_;
	std::unordered_map<Session_t::Id, std::shared_ptr<Session_t>> sessions_;
	std::queue<std::shared_ptr<Session_t>> free_sessions_;
	std::size_t next_session_id_;

	std::shared_ptr<makga::network::NetHandler> net_handler_;
	std::shared_ptr<makga::network::JobHandler> job_handler_;

};