#include "pch.h"
#include "iocp_server.h"

IocpServer::IocpServer(makga::network::IPEndPoint ep, std::shared_ptr<makga::network::IocpCore> core)
	: IocpService(makga::network::NetServiceType::IocpServer, core)
	, acceptor_(nullptr)
	, ep_(ep)
	, next_session_id_(0)
	, net_handler_(nullptr)
	, job_handler_(nullptr)
{
}

IocpServer::~IocpServer()
{
	Finalize();
}

bool IocpServer::Initialize(std::size_t max_connect_count, std::shared_ptr<NetHandler_t> net_handler, std::shared_ptr<JobHandler_t> job_handler)
{
	if (0 == max_connect_count)
	{
		makga::lib::MakgaLogger::Debug() << "IocpServer::Initialize failed.";
		//LOG_ERROR << "max_connect_count is 0.";
		return false;
	}

	if (nullptr == net_handler)
	{
		//LOG_ERROR << "net_handler is nullptr.";
		return false;
	}

	if (nullptr == job_handler)
	{
		//LOG_ERROR << "job_handler is nullptr.";
		return false;
	}

	CreateSession(max_connect_count);

	net_handler_ = net_handler;
	job_handler_ = job_handler;

	acceptor_ = std::make_shared<makga::network::IocpAcceptor>(shared_from_this());
	if (nullptr == acceptor_)
	{
		//LOG_ERROR << "acceptor_ is nullptr.";
		return false;
	}

	return acceptor_->Start();
}

void IocpServer::Finalize()
{
	if(nullptr != acceptor_)
	{
		acceptor_->Stop();
		acceptor_->Finalize();
		acceptor_ = nullptr;
	}

	if (nullptr != net_handler_)
	{
		net_handler_->Stop();
		net_handler_ = nullptr;
	}

	if (nullptr != job_handler_)
	{
		job_handler_->Stop();
		job_handler_ = nullptr;
	}

	DestroyAllSession();
}

std::shared_ptr<IocpServer::Session_t> IocpServer::FindSession(Session_t::Id id) const
{
	std::shared_lock lock(session_mutex_);

	auto iter = sessions_.find(id);
	if (iter != sessions_.end())
	{
		return iter->second;
	}

	return nullptr;
}

makga::network::IPEndPoint IocpServer::GetEndPoint() const
{
	return ep_;
}

std::size_t IocpServer::GetConnectCount() const
{
	std::shared_lock lock(session_mutex_);
	return sessions_.size();
}

std::size_t IocpServer::GetMaxConnectCount() const
{
	std::shared_lock lock(session_mutex_);
	return sessions_.size() + free_sessions_.size();
}

std::shared_ptr<IocpServer::Session_t> IocpServer::AllocSession()
{
	std::unique_lock lock(session_mutex_);

	if (true == free_sessions_.empty())
	{
		return nullptr;
	}

	auto session = free_sessions_.front();
	free_sessions_.pop();

	sessions_.emplace(session->GetSessionId(), session);
	return session;
}

void IocpServer::DeallocSession(std::shared_ptr<Session_t> session)
{
	if (nullptr == session)
	{
		return;
	}

	std::unique_lock lock(session_mutex_);
	sessions_.erase(session->GetSessionId());
	free_sessions_.push(session);
}

void IocpServer::CreateSession(std::size_t max_connect_count)
{
	if (0 == max_connect_count)
	{
		return;
	}

	DestroyAllSession();
	
	std::unique_lock lock(session_mutex_);

	for (std::size_t i = 0; i < max_connect_count; ++i)
	{
		auto session = std::make_shared<Session_t>(shared_from_this());
		session->SetSessionId(++next_session_id_);

		free_sessions_.push(session);
	}
}

void IocpServer::DestroyAllSession()
{
	std::unique_lock lock(session_mutex_);

	sessions_.clear();
	while (false == free_sessions_.empty())
	{
		free_sessions_.pop();
	}
}