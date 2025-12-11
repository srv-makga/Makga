
#include "WinSock2.h"
#include "Windows.h"
#include <memory>
#include <mutex>

module makga.network.iocp.acceptor;

import <vector>;
import <queue>;
import makga.network.iocp.service;
import makga.network.iocp.object;
import makga.network.iocp.session;
import makga.network.iocp.event;
import makga.network.socket;

namespace makga::network {
IocpAcceptor::IocpAcceptor(std::shared_ptr<IocpService> service)
	: service_(service)
{
}

IocpAcceptor::~IocpAcceptor()
{
}

bool IocpAcceptor::Initialize()
{
	return true;
}

void IocpAcceptor::Finalize()
{
	CloseSocket();
}

bool IocpAcceptor::RegisterAccept(IocpAcceptEvent* event)
{
	if (nullptr == event)
	{
		return false;
	}

	// Service에 등록은 Accept 이후 처리에서 된다
	std::shared_ptr<IocpSession> session = service_->AllocSession();

	event->session_ = session;

	DWORD bytes_received = 0;
	if (FALSE == SocketFunc::Instance().AcceptEx(socket_, session->GetSocket(), session->recv_buffer_->WritePosition(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes_received, static_cast<LPOVERLAPPED>(event)))
	{
		if (WSA_IO_PENDING != ::WSAGetLastError())
		{
			// LOG_WARN << "AcceptEx failed. ErrorCode: " << ::WSAGetLastError();

			event->session_ = nullptr;
			service_->DeallocSession(session);

			// @todo 혹시나 무한루프 가능성이 있나?
			RegisterAccept(event);
		}
	}

	return true;
}

void IocpAcceptor::ProcessAccept(IocpAcceptEvent* event)
{
	std::shared_ptr<IocpSession> session = event->session_;

	if (false == SetUpdateAcceptSocket(session->GetSocket(), socket_))
	{
		return;
	}

	SOCKADDR_IN session_addr{};
	int addrlen = sizeof(session_addr);
	::getpeername(session->GetSocket(), (SOCKADDR*)&session_addr, &addrlen);

	session->SetEndPoint(session_addr);
	session->OnConnect();
}

bool IocpAcceptor::CanAcceptSession() const
{
	return GetConnectCount() < GetMaxConnectCount();
}

HANDLE IocpAcceptor::GetHandle() const
{
	return reinterpret_cast<HANDLE>(socket_);
}

void IocpAcceptor::Dispatch(IocpEvent* iocp_event, int bytes_transferred)
{
	IocpAcceptEvent* accept_event = nullptr;

	do
	{
		if (IocpType::ACCEPT != iocp_event->type_)
		{
			break;
		}

		accept_event = static_cast<IocpAcceptEvent*>(iocp_event);
		ProcessAccept(accept_event);
	} while (false);

	do
	{
		if (false == CanAcceptSession())
		{
			break;
		}

		if (false == RegisterAccept(accept_event))
		{
			break;
		}

		// event 사용했으면 종료
		return;
	} while (false);

	PushFreeAcceptEvent(accept_event);
}

void IocpAcceptor::PushFreeAcceptEvent(IocpAcceptEvent* event)
{
	if (nullptr == event)
	{
		return;
	}

	std::unique_lock lock(mutex_);
	free_accept_events_.push(event);
}

IocpAcceptEvent* IocpAcceptor::PopFreeAcceptEvent()
{
	std::unique_lock lock(mutex_);
	if (true == free_accept_events_.empty())
	{
		return nullptr;
	}

	IocpAcceptEvent* event = free_accept_events_.front();
	free_accept_events_.pop();

	return event;
}
} // namespace makga::network