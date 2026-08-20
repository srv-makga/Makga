module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

module makga.network.iocp.service;

import <atomic>;
import <memory>;
import <utility>;

import makga.network.nethandler;
import makga.network.jobhandler;
import makga.network.service;
import makga.network.iocp.core;

namespace makga::network {
IocpService::IocpService(NetServiceType service_type, std::shared_ptr<IocpCore> core)
	: NetService(service_type)
	, core_(std::move(core))
{
}

IocpService::~IocpService()
{
	Finalize();
}

bool IocpService::Initialize()
{
	if (is_running_.load(std::memory_order_acquire))
		return true;
	if (core_ == nullptr)
		return false;
	if (!core_->Initialize())
		return false;

	is_running_.store(true, std::memory_order_release);
	return true;
}

void IocpService::Finalize()
{
	if (!is_running_.exchange(false, std::memory_order_acq_rel))
		return;

	// Handlers and core are injected shared dependencies. Do not reset their shared
	// ownership here; derived server/client instances may still need them for orderly drain.
	if (core_ != nullptr)
		core_->Finalize();
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
	return is_running_.load(std::memory_order_acquire);
}
}// namespace makga::network