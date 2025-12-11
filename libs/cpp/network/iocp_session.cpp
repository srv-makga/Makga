#include <WinSock2.h>
#include <Windows.h>
#include <memory>
#include <mutex>

module makga.network.iocp.session;

import <queue>;
import makga.network.endpoint;
import makga.network.buffer;
import makga.network.socket;
import makga.network.iocp.acceptor;
import makga.network.iocp.event;
import makga.network.iocp.object;
import makga.network.iocp.service;

namespace makga::network {
IocpSession::IocpSession(std::shared_ptr<IocpService> service)
	: id_(0)
	, socket_(INVALID_SOCKET)
	, is_connected_(false)
	, is_zero_receivce_(false)
	, recv_time_(0)
	, recv_size_(0)
	, sent_time_(0)
	, sent_size_(0)
	, is_send_registered_(false)
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

	if (INVALID_SOCKET == socket_)
	{
		::closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}

	is_connected_ = false;
	is_zero_receivce_ = false;
	recv_time_ = 0;
	recv_size_ = 0;
	sent_time_ = 0;
	sent_size_ = 0;

	{
		std::unique_lock lock(send_mutex_);

		while (false == send_buffer_queue_.empty())
		{
			send_buffer_queue_.pop();
		}
	}

	is_send_registered_ = false;

	service_ = nullptr;

	return true;
}

void IocpSession::Finalize()
{
	// @todo safe close socket
	if (INVALID_SOCKET == socket_)
	{
		::closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}

	recv_event_.owner_ = nullptr;
	connect_event_.owner_ = nullptr;
	disconnect_event_.owner_ = nullptr;	
	send_event_.owner_ = nullptr;

	service_ = nullptr;
}

bool IocpSession::Connect()
{
	if (true == is_connected_)
	{
		return false;
	}

	return RegisterConnect();
}

void IocpSession::Disconnect()
{
	if (false == is_connected_)
	{
		return;
	}

	RegisterDisconnect();
}

void IocpSession::Send(std::shared_ptr<NetBuffer> send_buffer)
{
	if (false == IsConnected())
	{
		return;
	}

	bool is_registered = false;

	{
		std::unique_lock lock(send_mutex_);

		send_buffer_queue_.push(send_buffer);
		if (false == is_send_registered_.exchange(true))
		{
			is_registered = true;
		}
	}

	// 다른 스레드에서 send 중이지 않으면 send 한다
	if (true == is_registered)
	{
		RegisterSend();
	}
}

int IocpSession::Recv()
{
	is_zero_receivce_ = false;

	if (false == IsConnected())
	{
		return false;
	}

	// @todo 링 버퍼는 버퍼와 남은 공간 계산을 이렇게 할 수 없다 수정 필요.
	int recv_size = ::recv(socket_, recv_buffer_->WritePosition(), static_cast<int>(recv_buffer_->AvailableWriteSize()), 0);
	if (0 >= recv_size)
	{
		return 0;
	}

	// @todo 수신 버퍼의 데이터를 처리하는 함수 실행

	return recv_size;
}

bool IocpSession::RegisterConnect()
{
	if (true == IsConnected())
	{
		return false;
	}

	if (NetServiceType::IocpClient != service_->GetServiceType())
	{
		return false;
	}

	if (false == SocketFunc::SetReuseAddr(socket_, true))
	{
		return false;
	}

	if (false == SocketFunc::BindAddrAny(socket_, 0))
	{
		return false;
	}

	connect_event_.Initialize();
	connect_event_.owner_ = shared_from_this();

	DWORD bytes_transferred = 0;
	SOCKADDR_IN sock_addr = service_->GetEndPoint().Addr(); // 어느 서버로 접속해야 하는지

	if (FALSE == SocketFunc::ConnectEx(socket_, reinterpret_cast<SOCKADDR*>(&sock_addr), sizeof(sock_addr), nullptr, 0, &bytes_transferred, &connect_event_))
	{
		if (ERROR_IO_PENDING != ::WSAGetLastError())
		{
			connect_event_.owner_ = nullptr;
			return false;
		}
	}

	return true;
}

void IocpSession::RegisterDisconnect()
{
	disconnect_event_.Initialize();
	disconnect_event_.owner_ = shared_from_this();

	if (FALSE == SocketFunc::Instance().DisconnectEx(socket_, &disconnect_event_, TF_REUSE_SOCKET, 0))
	{
		if (ERROR_IO_PENDING != ::WSAGetLastError())
		{
			disconnect_event_.owner_ = nullptr;
			return;
		}
	}
}

void IocpSession::RegisterZeroRecv()
{
	if (false == IsConnected())
	{
		return;
	}

	recv_event_.Initialize();
	recv_event_.owner_ = shared_from_this();

	WSABUF wsabuf{ 0, nullptr };

	int result = ::WSARecv(socket_, &wsabuf, 0, nullptr, nullptr, &recv_event_, nullptr);
}

void IocpSession::RegisterRecv()
{
	if (false == IsConnected())
	{
		return;
	}

	recv_event_.Initialize();
	recv_event_.owner_ = shared_from_this();

	WSABUF wsabuf{};
	wsabuf.buf = reinterpret_cast<CHAR*>(recv_buffer_->WritePosition());
	wsabuf.len = static_cast<ULONG>(recv_buffer_->AvailableWriteSize());

	DWORD recv_length = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(socket_, &wsabuf, 1, &recv_length, &flags, &recv_event_, nullptr))
	{
		if (ERROR_IO_PENDING != ::WSAGetLastError())
		{
			recv_event_.owner_ = nullptr;
			return;
		}
	}
}

void IocpSession::RegisterSend()
{
	if (false == IsConnected())
	{
		return;
	}

	send_event_.owner_ = shared_from_this();

	{
		std::unique_lock lock(send_mutex_);
		if (true == send_buffer_queue_.empty())
		{
			return;
		}

		while (false == send_buffer_queue_.empty())
		{
			std::shared_ptr<NetBuffer> send_buffer = send_buffer_queue_.front();
			send_buffer_queue_.pop();

			send_event_.send_buffer_.push_back(send_buffer);
		}
	}

	std::vector<WSABUF> wsabufs;
	wsabufs.reserve(send_event_.send_buffer_.size());

	for (std::shared_ptr<NetBuffer> send_buffer : send_event_.send_buffer_)
	{
		WSABUF wsabuf{};

		wsabuf.buf = send_buffer->ReadPosition();
		wsabuf.len = static_cast<ULONG>(send_buffer->UsingSize());
		wsabufs.push_back(wsabuf);
	}

	DWORD bytes_transferred = 0;
	if (SOCKET_ERROR == ::WSASend(socket_, wsabufs.data(), static_cast<DWORD>(wsabufs.size()), &bytes_transferred, 0, &send_event_, NULL))
	{
		if (ERROR_IO_PENDING != ::WSAGetLastError())
		{
			send_event_.owner_ = nullptr;
			send_event_.send_buffer_.clear();

			is_send_registered_.store(false);
			return;
		}
	}
}

void IocpSession::OnConnect()
{
	connect_event_.owner_ = nullptr;

	is_connected_ = true;

	//auto shared_this = std::static_pointer_cast<IocpSession>(shared_from_this());
	service_->AddSession(shared_from_this());

	ProcConnect();

	RegisterZeroRecv();
}

void IocpSession::OnDisconnect()
{
	disconnect_event_.owner_ = nullptr;

	ProcDisconnect();

	//auto shared_this = static_pointer_cast<IocpSession>(shared_from_this());
	service_->RemoveSession(shared_from_this());
}

void IocpSession::OnRecv(int bytes_transferred)
{
	recv_time_ = std::time(nullptr);
	recv_size_ += bytes_transferred;
	recv_event_.owner_ = shared_from_this();

	if (0 == bytes_transferred)
	{
		Disconnect();
		return;
	}

	// iocp 에서 데이터를 실제 기록했고, 그 만큼 오프셋 조정이 필요함
	recv_buffer_->AddWriteOffset(bytes_transferred);

	while (true)
	{
		// 데이터가 없음
		if (0 == recv_buffer_->UsingSize())
		{
			break;
		}

		std::size_t data_size = recv_buffer_->UsingSize();
		std::size_t proc_length = ProcRecv(recv_buffer_->ReadPosition(), data_size);
		if (0 > proc_length || data_size < proc_length || false == recv_buffer_->AddReadOffset(proc_length))
		{
			Disconnect();
			return;
		}
	}

	RegisterZeroRecv();
}

void IocpSession::OnSend(int bytes_transferred)
{
	send_event_.owner_ = nullptr;
	send_event_.send_buffer_.clear();

	if (0 == bytes_transferred)
	{
		Disconnect();
		return;
	}

	ProcSend(bytes_transferred);

	{
		std::unique_lock lock(send_mutex_);

		if (true == send_buffer_queue_.empty())
		{
			is_send_registered_.store(false);
			return;
		}
	}

	RegisterSend();
}

IocpSession::Id IocpSession::GetSessionId() const
{
	return id_;
}

void IocpSession::SetSessionId(Id id)
{
	id_ = id;
}

SOCKET IocpSession::GetSocket() const
{
	return socket_;
}

const IPEndPoint& IocpSession::GetEndPoint() const
{
	return ep_;
}
void IocpSession::SetEndPoint(SOCKADDR_IN addrin)
{
	ep_ = addrin;
}

void IocpSession::SetEndPoint(IPEndPoint ep)
{
	ep_ = ep;
}

std::shared_ptr<IocpService> IocpSession::GetService() const
{
	return service_;
}

bool IocpSession::IsConnected() const
{
	return is_connected_;
}

bool IocpSession::IsSendRegistered() const
{
	return is_send_registered_;
}

bool IocpSession::IsZeroReceive() const
{
	return is_zero_receivce_;
}

void IocpSession::Dispatch(IocpEvent* event, int32_t bytes_transferred)
{
	if (nullptr == event)
	{
		return;
	}

	switch (event->type_)
	{
	case IocpType::CONNECT:
	{
		OnConnect();
	}
	break;
	case IocpType::DISCONNECT:
	{
		OnDisconnect();
	}
	break;
	case IocpType::SEND:
	{
		OnSend(bytes_transferred);
	}
	break;
	case IocpType::RECV:
	{
		// zero recieive 처리
		if (0 == bytes_transferred)
		{
			if (false == IsZeroReceive() || 0 == Recv())
			{
				RegisterZeroRecv();
				return;
			}
		}

		OnRecv(bytes_transferred);
	}
	break;
	default:
	{
		// @todo 등록되지 않은 IOCP Event 타입 에러
	}
	break;
	}
}

HANDLE IocpSession::GetHandle() const
{
	return reinterpret_cast<HANDLE>(socket_);
}
} // namespace makga::network