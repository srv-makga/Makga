module;

#include "WinSock2.h"
#include "Windows.h"
#include <format>

module makga.network.iocp.connector;

import <memory>;
import <vector>;
import <queue>;
import makga.network.iocp.service;
import makga.network.iocp.object;
import makga.network.iocp.event;
import makga.network.session;
import makga.network.socket.util;
import makga.lib.lock;
import makga.lib.logger;

namespace makga::network {
IocpConnector::IocpConnector(std::shared_ptr<IocpService> service)
	: service_(service)
	, socket_(INVALID_SOCKET)
	, is_connected_(false)
{
}

IocpConnector::~IocpConnector()
{
	service_ = nullptr;

	Finalize();
}

bool IocpConnector::Initialize()
{
    if (service_ == nullptr || service_->GetIocpCore() == nullptr)
    {
        lib::MakgaLogger::Error("IocpConnector::Initialize - Service or IOCP core is nullptr.");
        return false;
    }

    is_connected_ = false;
    connect_event_.Initialize();
    return true;
}

void IocpConnector::Finalize()
{
	if (session_)
	{
		session_->BeginClose();
		SocketFunc::CloseSocket(socket_);
		session_->WaitForIoDrain();
	}
	else
	{
		SocketFunc::CloseSocket(socket_);
	}

	is_connected_ = false;
	session_ = nullptr;
}

bool IocpConnector::Start()
{
	if (true == is_connected_)
	{
		lib::MakgaLogger::Error("IocpConnector::Start - Already started.");
		return false;
	}

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

    if (!RegisterConnect(&connect_event_))
    {
        SocketFunc::CloseSocket(socket_);
        return false;
    }

	return true;
}

void IocpConnector::Stop()
{
	if (session_)
		session_->BeginClose();
	SocketFunc::CloseSocket(socket_);
	if (session_)
		session_->WaitForIoDrain();
	is_connected_ = false;
}

bool IocpConnector::RegisterConnect(IocpConnectEvent* event)
{
	if (nullptr == event)
	{
		return false;
	}

	std::shared_ptr<NetSession> session = service_->AllocSession();
	if (!session || !session->ResetIoState() || !session->TryBeginIo())
	{
		if (session) service_->DeallocSession(session);
		return false;
	}

	event->owner_ = shared_from_this();
	event->session_ = session;

	const auto& addr = service_->GetEndPoint().Addr();

	DWORD bytes_received = 0;
	if (FALSE == SocketFunc::ConnectEx(session->GetSocket(), (sockaddr*)&addr, sizeof(addr), nullptr, 0, nullptr, static_cast<LPOVERLAPPED>(event)))
	{
		if (WSA_IO_PENDING != ::WSAGetLastError())
		{
			lib::MakgaLogger::Error(std::format("IocpConnector::RegisterConnect - ConnectEx failed. ErrorCode: {0}", ::WSAGetLastError()));

            event->owner_ = nullptr;
            event->session_ = nullptr;
            session->CompleteIo();
            session->BeginClose();
            service_->DeallocSession(session);

            // 즉시 실패를 재귀적으로 재등록하면 지속적 오류에서 stack overflow가 발생한다.
            // 재접속은 호출자 또는 상위 reconnect scheduler가 결정한다.
            return false;
		}
	}

	return true;
}

void IocpConnector::ProcessConnect(IocpConnectEvent* event)
{
	if (event == nullptr || !event->session_)
		return;
	std::shared_ptr<NetSession> session = event->session_;
	if (session->IsClosing())
		return;
	SOCKADDR_IN session_addr{};
	int addrlen = sizeof(session_addr);
	::getpeername(session->GetSocket(), (SOCKADDR*)&session_addr, &addrlen);

	is_connected_ = true;

	session->SetEndPoint(session_addr);
	session->OnConnect();
}

HANDLE IocpConnector::GetHandle() const
{
    return socket_ == INVALID_SOCKET ? nullptr : reinterpret_cast<HANDLE>(socket_);
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
			if (connect_event->session_)
				connect_event->session_->CompleteIo();
			ProcessConnect(connect_event);
	} while (false);

	do
	{
		if (socket_ == INVALID_SOCKET || service_ == nullptr ||
			false == RegisterConnect(connect_event))
		{
			break;
		}

		return;
	} while (false);
}
} // namespace makga::network
