#include "stdafx.h"
#include "iocp_client.h"

IocpClient::IocpClient(makga::network::IPEndPoint ep, std::shared_ptr<makga::network::IocpCore> core)
	: IocpService(makga::network::NetServiceType::IocpClient, core)
	, connector_(nullptr)
	, ep_(ep)
	, session_(nullptr)
	, net_handler_(nullptr)
	, job_handler_(nullptr)
{
}

IocpClient::~IocpClient()
{
	Finalize();
}

bool IocpClient::Initialize(std::size_t max_connect_count, std::shared_ptr<NetHandler_t> net_handler, std::shared_ptr<JobHandler_t> job_handler)
{
	if (nullptr == net_handler)
	{
		makga::lib::MakgaLogger::Error() << "IocpClient::Initialize failed. net_handler is nullptr.";
		return false;
	}
	if (nullptr == job_handler)
	{
		makga::lib::MakgaLogger::Error() << "IocpClient::Initialize failed. job_handler is nullptr.";
		return false;
	}

	net_handler_ = net_handler;
	job_handler_ = job_handler;

	if (nullptr != session_)
	{
		CreateSession();
		session_ = AllocSession();
	}

	connector_ = std::make_shared<makga::network::IocpConnector>(shared_from_this());
	if (nullptr == connector_)
	{
		makga::lib::MakgaLogger::Error() << "IocpClient::Initialize failed. connector_ is nullptr.";
		return false;
	}

	if (false == connector_->Initialize())
	{
		makga::lib::MakgaLogger::Error() << "IocpClient::Initialize failed. connector_ initialize failed.";
		return false;
	}

	if (false == connector_->Start())
	{
		makga::lib::MakgaLogger::Error() << "IocpClient::Initialize failed. connector_ start failed.";
		return false;
	}

	return true;
}

void IocpClient::Finalize()
{
	if (nullptr != connector_)
	{
		connector_->Stop();
		connector_->Finalize();
		connector_ = nullptr;
	}

	net_handler_ = nullptr;
	job_handler_ = nullptr;

	session_ = nullptr;
}

std::shared_ptr<IocpClient::Session_t> IocpClient::GetSession() const
{
	return session_;
}

makga::network::IPEndPoint IocpClient::GetEndPoint() const
{
	return ep_;
}

std::size_t IocpClient::GetConnectCount() const
{
	if (nullptr == connector_)
	{
		return 0;
	}

	return (true == connector_->IsConnected()) ? 1 : 0;
}

std::size_t IocpClient::GetMaxConnectCount() const
{
	return std::size_t(1);
}

std::shared_ptr<IocpClient::Session_t> IocpClient::AllocSession()
{
	if (0 != GetConnectCount())
	{
		return nullptr;
	}

	return session_;
}


void IocpClient::DeallocSession(std::shared_ptr<Session_t> session)
{
	(void)session;
}