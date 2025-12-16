#pragma once

import makga.network.endpoint;
import makga.network.iocp.service;
import makga.network.iocp.connector;
import makga.network.iocp.session;
import makga.network.nethandler;
import makga.network.jobhandler;
import makga.lib.lock;

class IocpClient : public makga::network::IocpService, public std::enable_shared_from_this<IocpClient>
{
public:
	using Session_t = makga::network::IocpSession;
	using NetHandler_t = makga::network::NetHandler;
	using JobHandler_t = makga::network::JobHandler;

public:
	IocpClient(makga::network::IPEndPoint ep, std::shared_ptr<makga::network::IocpCore> core);
	virtual ~IocpClient();

	bool Initialize(std::size_t max_connect_count, std::shared_ptr<NetHandler_t> net_handler, std::shared_ptr<JobHandler_t> job_handler);
	void Finalize();

	std::shared_ptr<Session_t> GetSession() const;

public: // IocpService
	makga::network::IPEndPoint GetEndPoint() const override;
	std::size_t GetConnectCount() const override;
	std::size_t GetMaxConnectCount() const override;

	std::shared_ptr<Session_t> AllocSession() override;
	void DeallocSession(std::shared_ptr<Session_t> session) override;

protected:
	virtual void CreateSession(std::size_t max_connect_count = 1) = 0;
	void DestroyAllSession();

protected:
	makga::network::IPEndPoint ep_;
	std::shared_ptr<makga::network::IocpConnector> connector_;

	mutable makga::lib::SharedMutex session_mutex_;
	std::shared_ptr<Session_t> session_;

	std::shared_ptr<NetHandler_t> net_handler_;
	std::shared_ptr<JobHandler_t> job_handler_;
};