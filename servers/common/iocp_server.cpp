#include "pch.h"
#include "iocp_server.h"

IocpServer::IocpServer(std::shared_ptr<makga::network::IocpCore> core)
	: IocpService(makga::network::NetServiceType::IocpServer, core)
	, acceptor_(nullptr)
	, next_session_id_(0)
	, net_handler_(nullptr)
	, job_handler_(nullptr)
{
}

IocpServer::~IocpServer()
{
}

bool IocpServer::Initialize(std::size_t max_connect_count)
{
	CreateSession(max_connect_count);

	return true;
}

void IocpServer::Finalize()
{
	std::unique_lock lock(session_mutex_);
	sessions_.clear();

	while(false == free_sessions_.empty())
	{
		free_sessions_.pop();
	}
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

	std::unique_lock lock(session_mutex_);

	sessions_.clear();
	while (false == free_sessions_.empty())
	{
		free_sessions_.pop();
	}

	for (std::size_t i = 0; i < max_connect_count; ++i)
	{
		auto session = std::make_shared<Session_t>(shared_from_this());
		session->SetSessionId(static_cast<Session_t::Id>(++next_session_id_));

		free_sessions_.push(session);
	}
}