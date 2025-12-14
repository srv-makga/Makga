#include "WinSock2.h"
#include "Windows.h"
#include <memory>

module makga.network.iocp.connector;

import <vector>;
import <queue>;
import makga.network.iocp.service;
import makga.network.iocp.object;
import makga.network.iocp.event;
import makga.lib.lock;
import makga.lib.logger;

namespace makga::network {
IocpConnector::IocpConnector(std::shared_ptr<IocpService> service)
	: service_(service)
	, socket_(INVALID_SOCKET)
{
}

IocpConnector::~IocpConnector()
{
	service_ = nullptr;

	Finalize();
}

bool IocpConnector::Initialize()
{
	return true;
}

void IocpConnector::Finalize()
{
	SocketFunc::CloseSocket(socket_);

	lib::LockGuard lock(mutex_);

	for (IocpAcceptEvent* accept_event : accept_events_)
	{
		delete accept_event;
	}

	accept_events_.clear();

	while (false == free_accept_events_.empty())
	{
		IocpAcceptEvent* accept_event = free_accept_events_.front();
		delete accept_event;
		free_accept_events_.pop();
	}
}

bool IocpConnector::Start()
{
	if (nullptr == service_)
	{
		lib::MakgaLogger::Error("IocpConnector::Start - Service is nullptr.");
		return false;
	}

	SocketFunc::CloseSocket(socket_);

	socket_ = SocketFunc::Socket();
	if (INVALID_SOCKET == socket_)
	{
		lib::MakgaLogger::Error("IocpConnector::Start - Create socket failed.");
		return false;
	}

	if (false == service_->GetIocpCore()->Registered(reinterpret_cast<HANDLE>(socket_), 0))
	{
		lib::MakgaLogger::Error("IocpConnector::Start - Registered failed.");
		return false;
	}

	if (false == SocketFunc::SetReuseAddr(socket_, true))
	{
		lib::MakgaLogger::Error("IocpConnector::Start - Set reuse addr.");
		return false;
	}

	SocketFunc::SetLinger(socket_, 0, 0);
	if (false == SocketFunc::Bind(socket_, service_->GetEndPoint()))
	{
		lib::MakgaLogger::Error("IocpConnector::Start - Socket bind failed.");
		return false;
	}

	if (false == SocketFunc::Listen(socket_))
	{
		lib::MakgaLogger::Error("IocpConnector::Start - Socket listen failed.");
		return false;
	}

	lib::LockGuard lock(mutex_);

	accept_events_.clear();

	std::size_t max_session_count = service_->GetMaxConnectCount();
	for (std::size_t i = 0; i < max_session_count; ++i)
	{
		IocpAcceptEvent* accept_event = new IocpAcceptEvent();
		if (nullptr == accept_event)
		{
			lib::MakgaLogger::Error("IocpConnector::Start - Accept event create failed.");
			return false;
		}

		accept_events_.push_back(accept_event);

		accept_event->owner_ = shared_from_this();
		RegisterAccept(accept_event);
	}

	return true;
}

void IocpConnector::Stop()
{
	SocketFunc::CloseSocket(socket_);
}

bool IocpConnector::RegisterAccept(IocpAcceptEvent* event)
{
	if (nullptr == event)
	{
		return false;
	}

	std::shared_ptr<IocpSession> session = service_->AllocSession();

	event->session_ = session;

	DWORD bytes_received = 0;
	if (FALSE == SocketFunc::AcceptEx(socket_, session->GetSocket(), session->recv_buffer_->WritePosition(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes_received, static_cast<LPOVERLAPPED>(event)))
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

void IocpConnector::ProcessAccept(IocpAcceptEvent* event)
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

bool IocpConnector::CanAcceptSession() const
{
	return GetConnectCount() < GetMaxConnectCount();
}

HANDLE IocpConnector::GetHandle() const
{
	return reinterpret_cast<HANDLE>(socket_);
}

void IocpConnector::Dispatch(IocpEvent* iocp_event, int bytes_transferred)
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

void IocpConnector::PushFreeAcceptEvent(IocpAcceptEvent* event)
{
	if (nullptr == event)
	{
		return;
	}

	std::unique_lock lock(mutex_);
	free_accept_events_.push(event);
}

IocpAcceptEvent* IocpConnector::PopFreeAcceptEvent()
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