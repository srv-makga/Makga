#include "WinSock2.h"
#include "Windows.h"
#include <memory>

module makga.network.iocp.connector;

import <vector>;
import <queue>;
import <format>;
import makga.network.iocp.service;
import makga.network.iocp.object;
import makga.network.iocp.session;
import makga.network.iocp.event;
import makga.network.socket;
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
	connect_event_.Initialize();

	return true;
}

void IocpConnector::Finalize()
{
	SocketFunc::CloseSocket(socket_);

	session_ = nullptr;
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

	RegisterConnect(&connect_event_);

	return true;
}

void IocpConnector::Stop()
{
	SocketFunc::CloseSocket(socket_);
}

bool IocpConnector::RegisterConnect(IocpConnectEvent* event)
{
	if (nullptr == event)
	{
		return false;
	}

	std::shared_ptr<IocpSession> session = service_->AllocSession();

	event->owner_ = session;

	const auto& addr = service_->GetEndPoint().Addr();

	DWORD bytes_received = 0;
	if (FALSE == SocketFunc::ConnectEx(session->GetSocket(), (sockaddr*)&addr, sizeof(addr), nullptr, 0, nullptr, static_cast<LPOVERLAPPED>(event)))
	{
		if (WSA_IO_PENDING != ::WSAGetLastError())
		{
			lib::MakgaLogger::Error(std::format("IocpConnector::RegisterConnect - ConnectEx failed. ErrorCode: {0}", ::WSAGetLastError()));

			event->owner_ = nullptr;
			service_->DeallocSession(session);

			// @todo 혹시나 무한루프 가능성이 있나?
			RegisterConnect(event);
		}
	}

	return true;
}

void IocpConnector::ProcessConnect(IocpConnectEvent* event)
{
	SOCKADDR_IN session_addr{};
	int addrlen = sizeof(session_addr);
	::getpeername(session_->GetSocket(), (SOCKADDR*)&session_addr, &addrlen);

	session_->SetEndPoint(session_addr);
	session_->OnConnect();
}

HANDLE IocpConnector::GetHandle() const
{
	return reinterpret_cast<HANDLE>(session_->GetSocket());
}

void IocpConnector::Dispatch(IocpEvent* event, int bytes_transferred)
{
	IocpConnectEvent* connect_event = nullptr;

	do
	{
		if (IocpType::CONNECT != event->type_)
		{
			break;
		}

		connect_event = static_cast<IocpConnectEvent*>(event);
		ProcessConnect(connect_event);
	} while (false);

	do
	{
		if (false == RegisterConnect(connect_event))
		{
			break;
		}

		// event 사용했으면 종료
		return;
	} while (false);
}
} // namespace makga::network