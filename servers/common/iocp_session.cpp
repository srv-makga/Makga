#include "pch.h"
#include "iocp_session.h"
#include <algorithm>
#include <chrono>
#include <vector>

import <queue>;
import makga.lib.lock;
import makga.network.session;
import makga.network.endpoint;
import makga.network.socket.util;
import makga.network.iocp.acceptor;
import makga.network.iocp.event;
import makga.network.iocp.object;
import makga.network.iocp.service;

namespace {
std::uint64_t MonotonicTick()
{
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}
}

IocpSession::IocpSession(std::shared_ptr<makga::network::IocpService> service)
    : NetSession()
    , socket_(INVALID_SOCKET)
    , is_connected_(false)
    , is_zero_receivce_(false)
    , recv_time_(0)
    , recv_size_(0)
    , sent_time_(0)
    , sent_size_(0)
    , is_send_registered_(false)
    , pending_send_bytes_(0)
    , service_(service)
{
}

IocpSession::~IocpSession()
{
    Finalize();
}

bool IocpSession::Initialize()
{
    id_ = 0;
    if (socket_ != INVALID_SOCKET)
    {
        ::closesocket(socket_);
        socket_ = INVALID_SOCKET;
    }

    is_connected_.store(false, std::memory_order_release);
    is_zero_receivce_ = false;
    recv_time_.store(0, std::memory_order_release);
    recv_size_.store(0, std::memory_order_release);
    sent_time_.store(0, std::memory_order_release);
    sent_size_.store(0, std::memory_order_release);
    {
        makga::lib::LockGuard guard(send_mutex_);
        while (!send_buffer_queue_.empty()) send_buffer_queue_.pop();
        send_event_.send_buffer_.clear();
    }
    is_send_registered_.store(false, std::memory_order_release);
    pending_send_bytes_.store(0, std::memory_order_release);
    queued_send_bytes_.store(0, std::memory_order_release);
    next_batch_flush_tick_.store(0, std::memory_order_release);
    recv_buffer_ = std::make_shared<NetPacket>();
    recv_buffer_->AllocateBuffer(kInitialReceiveBufferBytes);
    return recv_buffer_->ValidBuffer();
}

void IocpSession::Finalize()
{
    is_connected_.store(false, std::memory_order_release);
    if (socket_ != INVALID_SOCKET)
    {
        ::closesocket(socket_);
        socket_ = INVALID_SOCKET;
    }

    {
        makga::lib::LockGuard guard(send_mutex_);
        while (!send_buffer_queue_.empty()) send_buffer_queue_.pop();
        send_event_.send_buffer_.clear();
    }
    pending_send_bytes_.store(0, std::memory_order_release);
    queued_send_bytes_.store(0, std::memory_order_release);
    next_batch_flush_tick_.store(0, std::memory_order_release);
    is_send_registered_.store(false, std::memory_order_release);
    recv_event_.owner_ = nullptr;
    connect_event_.owner_ = nullptr;
    disconnect_event_.owner_ = nullptr;
    send_event_.owner_ = nullptr;
    recv_buffer_.reset();
    service_ = nullptr;
}

bool IocpSession::Connect()
{
    return !IsConnected() && RegisterConnect();
}

void IocpSession::Disconnect()
{
    if (IsConnected()) RegisterDisconnect();
}

bool IocpSession::RequestGracefulClose(std::chrono::milliseconds timeout)
{
    ShutdownMode expected = ShutdownMode::Active;
    if (!shutdown_mode_.compare_exchange_strong(expected, ShutdownMode::Graceful,
        std::memory_order_acq_rel))
        return expected == ShutdownMode::Graceful;

    const auto now = static_cast<std::uint64_t>(MonotonicTick());
    graceful_deadline_tick_.store(now + static_cast<std::uint64_t>(timeout.count()),
        std::memory_order_release);
    return true;
}

void IocpSession::ForceClose()
{
    shutdown_mode_.store(ShutdownMode::Forced, std::memory_order_release);
    BeginClose();

    std::size_t dropped = 0;
    {
        std::lock_guard lock(send_mutex_);
        while (!send_buffer_queue_.empty())
        {
            if (auto& packet = send_buffer_queue_.front(); packet != nullptr)
                dropped += packet->UsingSize();
            send_buffer_queue_.pop();
        }
    }
    ReleasePendingSendBytes(dropped);
    queued_send_bytes_.store(0, std::memory_order_release);
    next_batch_flush_tick_.store(0, std::memory_order_release);

    if (socket_ != INVALID_SOCKET)
    {
        ::shutdown(socket_, SD_BOTH);
        ::CancelIoEx(reinterpret_cast<HANDLE>(socket_), nullptr);
    }
}
void IocpSession::RequestLifecycleClose()
{
    BeginClose();
}

bool IocpSession::IsIoLifecycleSafe() const
{
    return IsClosing() && PendingIoCount() == 0 && !zero_recv_posted_.load(std::memory_order_acquire) && !IsConnected();
}

bool IocpSession::SweepIoLifecycle()
{
    auto mode = shutdown_mode_.load(std::memory_order_acquire);
    if (mode == ShutdownMode::Active) return false;
    if (mode == ShutdownMode::Graceful)
    {
        const auto pending = pending_send_bytes_.load(std::memory_order_acquire);
        if (pending != 0)
        {
            if (static_cast<std::uint64_t>(MonotonicTick()) < graceful_deadline_tick_.load(std::memory_order_acquire)) return false;
            ForceClose();
            mode = ShutdownMode::Forced;
        }
        else
        {
            BeginClose();
        }
    }
    if (!IsClosing()) return false;

    // A zero-byte receive is readiness-only. Cancel it before a shutdown,
    // but keep the socket alive until cancellation completion is observed.
    if (zero_recv_posted_.load(std::memory_order_acquire) && IsZeroReceive())
    {
        ::CancelIoEx(reinterpret_cast<HANDLE>(socket_), reinterpret_cast<OVERLAPPED*>(&recv_event_));
        return false;
    }

    if (!PendingIoCount() == 0) return false;
    if (IsConnected() && !disconnect_posted_.exchange(true, std::memory_order_acq_rel))
    {
        RegisterDisconnect();
        return false;
    }
    return IsIoLifecycleSafe();
}

void IocpSession::Send(std::shared_ptr<NetPacket> send_buffer)
{
    SendWithPriority(std::move(send_buffer), SendPriority::Normal);
}

bool IocpSession::SendWithPriority(std::shared_ptr<NetPacket> send_buffer, SendPriority priority)
{
    if (send_buffer == nullptr || !IsConnected()) return false;

    const std::size_t bytes = send_buffer->UsingSize();
    if (bytes == 0 || bytes > kMaxPendingSendBytes) return false;

    std::size_t current = pending_send_bytes_.load(std::memory_order_relaxed);
    for (;;)
    {
        const std::size_t limit = priority == SendPriority::Low ? kSoftPendingSendBytes :
            (priority == SendPriority::Normal ? kHardPendingSendBytes : kMaxPendingSendBytes);
        if (current > limit - bytes)
        {
            if (priority == SendPriority::Critical) Disconnect();
            return false;
        }
        if (pending_send_bytes_.compare_exchange_weak(current, current + bytes, std::memory_order_acq_rel)) break;
    }

    bool flush_now = priority == SendPriority::Critical || bytes >= kBatchFlushBytes;
    const auto now = static_cast<std::uint64_t>(MonotonicTick());
    {
        std::unique_lock lock(send_mutex_);
        if (!IsConnected())
        {
            ReleasePendingSendBytes(bytes);
            return false;
        }
        send_buffer_queue_.push(std::move(send_buffer));
        const std::size_t queued = queued_send_bytes_.fetch_add(bytes, std::memory_order_acq_rel) + bytes;
        if (queued >= kBatchFlushBytes) flush_now = true;
        std::uint64_t expected = 0;
        next_batch_flush_tick_.compare_exchange_strong(expected, now + kBatchFlushDelayMs,
            std::memory_order_acq_rel, std::memory_order_acquire);
        if (flush_now) next_batch_flush_tick_.store(0, std::memory_order_release);
    }

    if (service_ != nullptr) service_->ScheduleSendBatch(GetSessionId());
    if (flush_now) FlushSendBatch(now);
    return true;
}

bool IocpSession::EnsureReceiveCapacity(std::size_t required_write_bytes)
{
    if (required_write_bytes == 0 || required_write_bytes > kMaxReceiveBufferBytes) return false;
    if (recv_buffer_ == nullptr)
    {
        recv_buffer_ = std::make_shared<NetPacket>();
        recv_buffer_->AllocateBuffer(std::max(kInitialReceiveBufferBytes, required_write_bytes));
        return recv_buffer_->ValidBuffer();
    }

    if (recv_buffer_->AvailableWriteSize() >= required_write_bytes) return true;
    recv_buffer_->PullBuffer();
    if (recv_buffer_->AvailableWriteSize() >= required_write_bytes) return true;

    const std::size_t unread = recv_buffer_->UsingSize();
    if (unread > kMaxReceiveBufferBytes - required_write_bytes) return false;
    std::size_t next_capacity = std::max(recv_buffer_->BufferSize(), kInitialReceiveBufferBytes);
    const std::size_t required_capacity = unread + required_write_bytes;
    while (next_capacity < required_capacity && next_capacity < kMaxReceiveBufferBytes)
        next_capacity = std::min(kMaxReceiveBufferBytes, next_capacity * 2);
    if (next_capacity < required_capacity) return false;

    auto grown = std::make_shared<NetPacket>();
    grown->AllocateBuffer(next_capacity);
    if (unread != 0 && grown->Write(recv_buffer_->ReadPosition(), unread) != unread) return false;
    recv_buffer_ = std::move(grown);
    return true;
}

int IocpSession::Recv()
{
    is_zero_receivce_ = false;
    if (!IsConnected() || !EnsureReceiveCapacity(1)) return -1;

    const int received = ::recv(socket_, recv_buffer_->WritePosition(),
        static_cast<int>(recv_buffer_->AvailableWriteSize()), 0);
    if (received > 0) return received;
    if (received == 0) return -1;
    return ::WSAGetLastError() == WSAEWOULDBLOCK ? 0 : -1;
}

bool IocpSession::RegisterConnect()
{
    if (!TryBeginIo()) return false;
    if (IsConnected() || service_ == nullptr ||
        makga::network::NetServiceType::IocpClient != service_->GetServiceType())
    {
        CompleteIo();
        return false;
    }
    if (!makga::network::SocketFunc::SetReuseAddr(socket_, true) ||
        !makga::network::SocketFunc::BindAddrAny(socket_, 0))
    {
        CompleteIo();
        return false;
    }

    connect_event_.Initialize();
    connect_event_.owner_ = shared_from_this();
    DWORD bytes = 0;
    sockaddr_in address = service_->GetEndPoint().Addr();
    if (FALSE == makga::network::SocketFunc::ConnectEx(socket_, reinterpret_cast<sockaddr*>(&address),
        sizeof(address), nullptr, 0, &bytes, &connect_event_))
    {
        if (ERROR_IO_PENDING != ::WSAGetLastError())
        {
            connect_event_.owner_ = nullptr;
            CompleteIo();
            return false;
        }
    }
    return true;
}

void IocpSession::RegisterDisconnect()
{
    if (!TryBeginIo()) return;
    disconnect_event_.Initialize();
    disconnect_event_.owner_ = shared_from_this();
    if (FALSE == makga::network::SocketFunc::DisconnectEx(socket_, &disconnect_event_, TF_REUSE_SOCKET, 0) &&
        ERROR_IO_PENDING != ::WSAGetLastError())
    {
        disconnect_event_.owner_ = nullptr;
        CompleteIo();
    }
}

void IocpSession::RegisterZeroRecv()
{
    if (!IsConnected() || IsClosing()) return;

    is_zero_receivce_ = true;
    zero_recv_posted_.store(true, std::memory_order_release);
    recv_event_.Initialize();
    recv_event_.owner_ = shared_from_this();
    WSABUF buffer{0, nullptr};
    if (SOCKET_ERROR == ::WSARecv(socket_, &buffer, 0, nullptr, nullptr, &recv_event_, nullptr) &&
        ERROR_IO_PENDING != ::WSAGetLastError())
    {
        recv_event_.owner_ = nullptr;
    }
}

void IocpSession::RegisterRecv()
{
    if (!IsConnected() || !EnsureReceiveCapacity(1))
    {
        Disconnect();
        return;
    }

    is_zero_receivce_ = false;
    recv_event_.Initialize();
    recv_event_.owner_ = shared_from_this();
    WSABUF buffer{};
    buffer.buf = reinterpret_cast<CHAR*>(recv_buffer_->WritePosition());
    buffer.len = static_cast<ULONG>(recv_buffer_->AvailableWriteSize());
    DWORD flags = 0;
    if (SOCKET_ERROR == ::WSARecv(socket_, &buffer, 1, nullptr, &flags, &recv_event_, nullptr) &&
        ERROR_IO_PENDING != ::WSAGetLastError())
    {
        recv_event_.owner_ = nullptr;
    }
}

bool IocpSession::FlushSendBatch(std::uint64_t now_tick)
{
    if (!IsConnected() || IsClosing() || queued_send_bytes_.load(std::memory_order_acquire) == 0)
        return false;
    const std::uint64_t deadline = next_batch_flush_tick_.load(std::memory_order_acquire);
    if (queued_send_bytes_.load(std::memory_order_acquire) < kBatchFlushBytes && deadline != 0 && now_tick < deadline)
        return false;

    bool expected = false;
    if (!is_send_registered_.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        return false;
    next_batch_flush_tick_.store(0, std::memory_order_release);
    RegisterSend();
    return true;
}

void IocpSession::RegisterSend()
{
    if (!IsConnected())
    {
        is_send_registered_.store(false, std::memory_order_release);
        return;
    }

    if (!TryBeginIo()) return;
    send_event_.owner_ = shared_from_this();
    {
        std::unique_lock lock(send_mutex_);
        std::size_t batch_bytes = 0;
        while (!send_buffer_queue_.empty() && send_event_.send_buffer_.size() < kMaxBuffersPerSend)
        {
            auto packet = send_buffer_queue_.front();
            send_buffer_queue_.pop();
            if (packet != nullptr && packet->UsingSize() != 0)
            {
                batch_bytes += packet->UsingSize();
                send_event_.send_buffer_.push_back(std::move(packet));
            }
        }
        if (batch_bytes != 0)
        {
            const std::size_t previous = queued_send_bytes_.fetch_sub(batch_bytes, std::memory_order_acq_rel);
            if (previous < batch_bytes) queued_send_bytes_.store(0, std::memory_order_release);
        }

        if (send_event_.send_buffer_.empty())
        {
            send_event_.owner_ = nullptr;
            is_send_registered_.store(false, std::memory_order_release);
            return;
        }
    }

    std::vector<WSABUF> buffers;
    buffers.reserve(send_event_.send_buffer_.size());
    for (const auto& packet : send_event_.send_buffer_)
        buffers.push_back({ static_cast<ULONG>(packet->UsingSize()), packet->ReadPosition() });

    DWORD transferred = 0;
    if (SOCKET_ERROR == ::WSASend(socket_, buffers.data(), static_cast<DWORD>(buffers.size()),
        &transferred, 0, &send_event_, nullptr) && ERROR_IO_PENDING != ::WSAGetLastError())
    {
        std::size_t dropped = 0;
        for (const auto& packet : send_event_.send_buffer_)
            if (packet != nullptr) dropped += packet->UsingSize();
        send_event_.owner_ = nullptr;
        send_event_.send_buffer_.clear();
        ReleasePendingSendBytes(dropped);
        CompleteIo();
        is_send_registered_.store(false, std::memory_order_release);
        Disconnect();
    }
}

void IocpSession::OnConnect()
{
    connect_event_.owner_ = nullptr;
    CompleteIo();
    is_connected_.store(true, std::memory_order_release);
    ProcConnect();
    RegisterZeroRecv();
}

void IocpSession::OnDisconnect()
{
    disconnect_event_.owner_ = nullptr;
    CompleteIo();
    is_connected_.store(false, std::memory_order_release);
    ProcDisconnect();
}

void IocpSession::OnRecv(int bytes_transferred)
{
    recv_time_.store(std::time(nullptr), std::memory_order_release);
    recv_event_.owner_ = nullptr;
    if (is_zero_receivce_ && bytes_transferred == 0)
    {
        // Readiness-only completion: keep the socket alive and post a real receive.
        zero_recv_posted_.store(false, std::memory_order_release);
        RegisterRecv();
        return;
    }
    if (bytes_transferred <= 0)
    {
        Disconnect();
        return;
    }
    recv_size_.fetch_add(static_cast<std::size_t>(bytes_transferred), std::memory_order_acq_rel);

    recv_buffer_->AddWriteOffset(static_cast<std::size_t>(bytes_transferred));
    while (recv_buffer_->UsingSize() != 0)
    {
        const auto available = recv_buffer_->UsingSize();
        const auto consumed = ProcRecv(recv_buffer_->ReadPosition(), available);
        if (consumed == 0) break;
        if (consumed > available)
        {
            Disconnect();
            return;
        }
        recv_buffer_->AddReadOffset(consumed);
    }
    RegisterZeroRecv();
}

void IocpSession::OnSend(int bytes_transferred)
{
    send_event_.owner_ = nullptr;
    CompleteIo();
    if (bytes_transferred <= 0)
    {
        Disconnect();
        return;
    }

    std::size_t remaining = static_cast<std::size_t>(bytes_transferred);
    std::size_t consumed = 0;
    auto& inflight = send_event_.send_buffer_;
    while (!inflight.empty() && remaining != 0)
    {
        auto& packet = inflight.front();
        if (packet == nullptr || packet->UsingSize() == 0)
        {
            inflight.erase(inflight.begin());
            continue;
        }

        const std::size_t take = std::min(remaining, packet->UsingSize());
        packet->AddReadOffset(take);
        remaining -= take;
        consumed += take;
        if (packet->UsingSize() == 0) inflight.erase(inflight.begin());
    }

    if (remaining != 0)
    {
        Disconnect();
        return;
    }

    ReleasePendingSendBytes(consumed);
    ProcSend(bytes_transferred);
    {
        std::unique_lock lock(send_mutex_);
        if (inflight.empty())
        {
            is_send_registered_.store(false, std::memory_order_release);
            if (!send_buffer_queue_.empty())
            {
                const auto now = static_cast<std::uint64_t>(MonotonicTick());
                next_batch_flush_tick_.store(now + kBatchFlushDelayMs, std::memory_order_release);
                if (service_ != nullptr) service_->ScheduleSendBatch(GetSessionId());
            }
            return;
        }
    }
    // Partial WSASend: preserve the residual vectors and submit them without a batching delay.
    RegisterSend();
}

SOCKET IocpSession::GetSocket() const { return socket_; }
std::shared_ptr<makga::network::IocpService> IocpSession::GetService() const { return service_; }
bool IocpSession::IsConnected() const { return is_connected_.load(std::memory_order_acquire); }
bool IocpSession::IsSendRegistered() const { return is_send_registered_.load(std::memory_order_acquire); }
bool IocpSession::IsZeroReceive() const { return is_zero_receivce_; }
HANDLE IocpSession::GetHandle() const { return reinterpret_cast<HANDLE>(socket_); }
char* IocpSession::GetWritePosition() { return recv_buffer_ != nullptr ? recv_buffer_->WritePosition() : nullptr; }

void IocpSession::Dispatch(makga::network::IocpEvent* event, int32_t bytes_transferred)
{
    if (event == nullptr) return;

    switch (event->type_)
    {
    case makga::network::IocpType::CONNECT:
        OnConnect();
        break;
    case makga::network::IocpType::DISCONNECT:
        OnDisconnect();
        break;
    case makga::network::IocpType::SEND:
        OnSend(bytes_transferred);
        break;
    case makga::network::IocpType::RECV:
        if (IsZeroReceive()) zero_recv_posted_.store(false, std::memory_order_release);
        if (bytes_transferred == 0 && IsZeroReceive())
        {
            const int received = Recv();
            if (received > 0) OnRecv(received);
            else if (received == 0) RegisterZeroRecv();
            else Disconnect();
        }
        else
        {
            OnRecv(bytes_transferred);
        }
        break;
    default:
        break;
    }
}
