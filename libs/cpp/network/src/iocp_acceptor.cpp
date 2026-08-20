module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <algorithm>
#include <new>
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

module makga.network.iocp.acceptor;

import <atomic>;
import <condition_variable>;
import <cstddef>;
import <memory>;
import <queue>;
import <thread>;
import <vector>;



import makga.network.socket.util;
import makga.network.session;
import makga.network.acceptor;

import makga.network.iocp.service;
import makga.network.iocp.event;
import makga.network.iocp.object;

import makga.lib.lock;
import makga.lib.logger;

namespace makga::network {
IocpAcceptor::IocpAcceptor(std::shared_ptr<IocpService> service)
	: service_(service)
	, socket_(INVALID_SOCKET)
{
}

IocpAcceptor::~IocpAcceptor()
{
	service_ = nullptr;

	Finalize();
}

bool IocpAcceptor::Initialize()
{
    if (service_ == nullptr || service_->GetIocpCore() == nullptr)
    {
        lib::MakgaLogger::Error("IocpAcceptor::Initialize - Service or IOCP core is nullptr.");
        return false;
    }

    // 이전 Start/Stop cycle의 잔여 socket이 있으면 완료된 AcceptEx를 먼저 drain한다.
    if (socket_ != INVALID_SOCKET)
        Stop();

    return true;
}

void IocpAcceptor::Finalize()
{
    SocketFunc::CloseSocket(socket_);
    std::unique_lock drain_lock(mutex_);
    accept_drain_cv_.wait(drain_lock, [this]
    {
        return pending_accepts_.load(std::memory_order_acquire) == 0;
    });

    // pending AcceptEx가 모두 완료된 뒤에만 event를 해제한다.
	for (IocpAcceptEvent* accept_event : accept_events_)
	{
		delete accept_event;
	}

    accept_events_.clear();

    while (!free_accept_events_.empty())
    {
        delete free_accept_events_.front();
        free_accept_events_.pop();
    }
}

bool IocpAcceptor::Start()
{
    if (nullptr == service_ || nullptr == service_->GetIocpCore())
    {
        lib::MakgaLogger::Error("IocpAcceptor::Start - Service or IOCP core is nullptr.");
        return false;
    }

    SocketFunc::CloseSocket(socket_);
    socket_ = SocketFunc::Socket();
    if (socket_ == INVALID_SOCKET)
    {
        lib::MakgaLogger::Error("IocpAcceptor::Start - Create socket failed.");
        return false;
    }

    if (!service_->GetIocpCore()->Registered(reinterpret_cast<HANDLE>(socket_), 0) ||
        !SocketFunc::SetReuseAddr(socket_, true) ||
        !SocketFunc::Bind(socket_, service_->GetEndPoint()) ||
        !SocketFunc::Listen(socket_))
    {
        lib::MakgaLogger::Error("IocpAcceptor::Start - Socket setup failed.");
        SocketFunc::CloseSocket(socket_);
        return false;
    }

    lib::LockGuard lock(mutex_);
    if (!accept_events_.empty())
    {
        lib::MakgaLogger::Error("IocpAcceptor::Start - Acceptor is already started.");
        SocketFunc::CloseSocket(socket_);
        return false;
    }

    const auto logical_processors = std::max(1u, std::thread::hardware_concurrency());
    const auto accept_prepost_count = std::min<std::size_t>(
        service_->GetMaxConnectCount(), std::max<std::size_t>(64, static_cast<std::size_t>(logical_processors) * 8));
    std::size_t posted_accepts = 0;
    for (std::size_t i = 0; i < accept_prepost_count; ++i)
    {
        IocpAcceptEvent* accept_event = new (std::nothrow) IocpAcceptEvent();
        if (accept_event == nullptr)
            break;

        accept_event->owner_ = shared_from_this();
        accept_events_.push_back(accept_event);
        if (!RegisterAccept(accept_event))
        {
            accept_events_.pop_back();
            accept_event->owner_ = nullptr;
            delete accept_event;
            break;
        }
        ++posted_accepts;
    }

    if (posted_accepts == 0)
    {
        lib::MakgaLogger::Error("IocpAcceptor::Start - No AcceptEx request was posted.");
        SocketFunc::CloseSocket(socket_);
        return false;
    }

    return true;
}
void IocpAcceptor::Stop()
{
	SocketFunc::CloseSocket(socket_);
	std::unique_lock lock(mutex_);
	accept_drain_cv_.wait(lock, [this]
	{
		return pending_accepts_.load(std::memory_order_acquire) == 0;
	});
}

bool IocpAcceptor::RegisterAccept(IocpAcceptEvent* event)
{
	if (event == nullptr || service_ == nullptr || socket_ == INVALID_SOCKET)
		return false;

	std::shared_ptr<NetSession> session = service_->AllocSession();
	if (!session || !session->ResetIoState() || !session->TryBeginIo())
	{
		if (session) service_->DeallocSession(session);
		return false;
	}

	event->session_ = session;
	pending_accepts_.fetch_add(1, std::memory_order_acq_rel);

	DWORD bytes_received = 0;
	if (SocketFunc::AcceptEx(socket_, session->GetSocket(), session->GetWritePosition(), 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes_received,
		static_cast<LPOVERLAPPED>(event)))
	{
		return true;
	}

	if (WSA_IO_PENDING == ::WSAGetLastError())
		return true;

	const auto previous = pending_accepts_.fetch_sub(1, std::memory_order_acq_rel);
	if (previous == 1)
		accept_drain_cv_.notify_all();
	event->session_ = nullptr;
	session->CompleteIo();
	session->BeginClose();
	service_->DeallocSession(session);
	return false;
}

void IocpAcceptor::ProcessAccept(IocpAcceptEvent* event)
{
	if (event == nullptr)
		return;
	std::shared_ptr<NetSession> session = event->session_;
	if (session)
		session->CompleteIo();
	if (!session || session->IsClosing() ||
		false == SetUpdateAcceptSocket(session->GetSocket(), socket_))
	{
		if (session)
		{
			session->BeginClose();
			service_->DeallocSession(session);
		}
		event->session_ = nullptr;
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

std::size_t IocpAcceptor::GetConnectCount() const
{
	return service_ ? service_->GetConnectCount() : 0;
}

std::size_t IocpAcceptor::GetMaxConnectCount() const
{
	return service_ ? service_->GetMaxConnectCount() : 0;
}

HANDLE IocpAcceptor::GetHandle() const
{
	return reinterpret_cast<HANDLE>(socket_);
}

void IocpAcceptor::Dispatch(IocpEvent* iocp_event, int bytes_transferred)
{
	if (iocp_event == nullptr || IocpType::ACCEPT != iocp_event->type_)
		return;

	IocpAcceptEvent* accept_event = static_cast<IocpAcceptEvent*>(iocp_event);
	ProcessAccept(accept_event);

	// completion 처리와 event 재등록/반납까지를 하나의 outstanding 작업으로 본다.
	bool rearmed = false;
	if (CanAcceptSession() && RegisterAccept(accept_event))
		rearmed = true;

	const auto previous = pending_accepts_.fetch_sub(1, std::memory_order_acq_rel);
	if (previous == 1)
		accept_drain_cv_.notify_all();

	if (!rearmed)
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
