module makga.network.iocp.service;

import makga.network.nethandler;
import makga.network.jobhandler;
import makga.network.service;
import makga.network.iocp.core;

namespace makga::network {
IocpService::IocpService(std::shared_ptr<IocpCore> core)
	: NetService(NetServiceType::Iocp)
	, core_(core)
{
}

IocpService::~IocpService()
{
	Finalize();
}

bool IocpService::Initialize()
{
	core_ = nullptr;
	job_handler_ = nullptr;
	net_handler_ = nullptr;
	return true;
}

void IocpService::Finalize()
{
	core_ = nullptr;
	job_handler_ = nullptr;
	net_handler_ = nullptr;
}

std::shared_ptr<IocpCore> IocpService::GetIocpCore() const
{
	return core_;
}

std::shared_ptr<NetHandler> IocpService::GetNetHandler() const
{
	return net_handler_;
}

void IocpService::SetNetHandler(std::shared_ptr<NetHandler> handler)
{
	net_handler_ = handler;
}

std::shared_ptr<JobHandler> IocpService::GetJobHandler() const
{
	return job_handler_;
}

void IocpService::SetJobHandler(std::shared_ptr<JobHandler> handler)
{
	job_handler_ = handler;
}

bool IocpService::IsRunning() const
{
	return is_running_;
}
}// namespace makga::network