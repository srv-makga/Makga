#include "pch.h"
#include "net_thread.h"

import makga.network.iocp.core;
import makga.network.iocp.event;
import makga.network.iocp.object;
import makga.network.iocp.session;
import makga.network.nethandler;
import makga.lib.thread.group;

NetThread::NetThread(std::shared_ptr<makga::network::IocpCore> core)
	: core_(core)
	, thread_count_(1)
{
}

NetThread::~NetThread()
{
}

bool NetThread::Initialize()
{
	return true;
}

void NetThread::Finalize()
{
	thread_group_.Stop();
	// @todo 대기 필요
}

void NetThread::Run()
{
	if (nullptr == core_)
	{
		return;
	}

	ULONG_PTR completion_key = 0;
	DWORD transferred_bytes = 0;
	OVERLAPPED* overlapped = nullptr;
	int wsa_error = 0;

	while (true)
	{
		if (false == core_->GetStatus(&completion_key, transferred_bytes, &overlapped, INFINITE, wsa_error))
		{
			switch (wsa_error)
			{
			case WAIT_TIMEOUT:
			case ERROR_NETNAME_DELETED:
			case ERROR_OPERATION_ABORTED:
			case ERROR_SEM_TIMEOUT:
				continue; // 계속 대기
			default:
				Logger::Error() << "GetStatus failed. error code: " << wsa_error;
			}
		}

		if (nullptr == overlapped)
		{
			Logger::Error() << "Overlapped is null. completion_key: " << completion_key;
			continue;
		}

		makga::network::IocpEvent* context = static_cast<makga::network::IocpEvent*>(overlapped);

		switch (context->type_)
		{
		case makga::network::IocpType::ACCEPT: // owner: Acceptor
		case makga::network::IocpType::CONNECT: // owner: Session
		case makga::network::IocpType::DISCONNECT: // owner: Session
		case makga::network::IocpType::SEND: // owner: Session
		case makga::network::IocpType::RECV: // owner: Session
		{
			if (nullptr == context->owner_)
			{
				Logger::Error() << "IocpEvent owner is nullptr. completion_key: " << completion_key;
				continue;
			}

			context->owner_->Dispatch(context, transferred_bytes);
		}
		break;
		}
	}
}

bool NetThread::CreateThread(std::size_t thread_count)
{
	thread_count_ = thread_count;
}

bool NetThread::Start()
{
	thread_group_.Start([this]() { this->Run(); }, thread_count_);
}

void NetThread::Stop()
{
	thread_group_.Stop();
}

