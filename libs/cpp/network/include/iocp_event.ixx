module;

#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <memory>

export module makga.network.iocp.event;

namespace makga::network {
enum class IocpType
{
	ACCEPT,
	CONNECT,
	DISCONNECT,
	SEND,
	RECV
};

class IocpSession;
class IocpObject;
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent() = delete;
	IocpEvent(IocpType type)
		: type_(type)
		, owner_(nullptr)
	{
	}
	IocpEvent(const IocpEvent&) = delete;
	IocpEvent(IocpEvent&& other) = delete;
	IocpEvent& operator=(const IocpEvent&) = delete;
	IocpEvent& operator=(IocpEvent&&) = delete;
	virtual ~IocpEvent() = default;

	bool Initialize()
	{
		owner_ = nullptr;
		return true;
	}

	void Finalize()
	{
		owner_ = nullptr;
	}

	IocpType type_;
	std::shared_ptr<IocpObject> owner_;
};

class IocpAcceptEvent : public IocpEvent
{
public:
	IocpAcceptEvent() : IocpEvent(IocpType::ACCEPT) {}
	IocpAcceptEvent(const IocpAcceptEvent&) = delete;
	IocpAcceptEvent(IocpAcceptEvent&& other) = delete;
	IocpAcceptEvent& operator=(const IocpAcceptEvent&) = delete;
	IocpAcceptEvent& operator=(IocpAcceptEvent&&) = delete;
	virtual ~IocpAcceptEvent() = default;

public:
	std::shared_ptr<IocpSession> session_ = nullptr; // accept된 session
};

class IocpConnectEvent : public IocpEvent
{
public:
	IocpConnectEvent() : IocpEvent(IocpType::CONNECT) {}
	IocpConnectEvent(const IocpConnectEvent&) = delete;
	IocpConnectEvent(IocpConnectEvent&&) = delete;
	IocpConnectEvent& operator=(const IocpConnectEvent&) = delete;
	IocpConnectEvent& operator=(IocpConnectEvent&&) = delete;
	virtual ~IocpConnectEvent() = default;
};

class IocpDisconnectEvent : public IocpEvent
{
public:
	IocpDisconnectEvent() : IocpEvent(IocpType::DISCONNECT) {}
	IocpDisconnectEvent(const IocpDisconnectEvent&) = delete;
	IocpDisconnectEvent(IocpDisconnectEvent&&) = delete;
	IocpDisconnectEvent& operator=(const IocpDisconnectEvent&) = delete;
	IocpDisconnectEvent& operator=(IocpDisconnectEvent&&) = delete;
	virtual ~IocpDisconnectEvent() = default;
};

class IocpRecvEvent : public IocpEvent
{
public:
	IocpRecvEvent() : IocpEvent(IocpType::RECV) {}
	IocpRecvEvent(const IocpRecvEvent&) = delete;
	IocpRecvEvent(IocpRecvEvent&&) = delete;
	IocpRecvEvent& operator=(const IocpRecvEvent&) = delete;
	IocpRecvEvent& operator=(IocpRecvEvent&&) = delete;
	virtual ~IocpRecvEvent() = default;
};

template<typename T>
class IocpSendEvent : public IocpEvent
{
public:
	IocpSendEvent() : IocpEvent(IocpType::SEND) {}
	IocpSendEvent(const IocpSendEvent&) = delete;
	IocpSendEvent(IocpSendEvent&&) = delete;
	IocpSendEvent& operator=(const IocpSendEvent&) = delete;
	IocpSendEvent& operator=(IocpSendEvent&&) = delete;
	virtual ~IocpSendEvent() = default;

	std::vector<std::shared_ptr<T>> send_buffer_; // T는 전송될 데이터가 있는 네트워크용 버퍼
};
} // namespace makga::network