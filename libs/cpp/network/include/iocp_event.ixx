module;

#include <vector>
#include <memory>
#include <winsock2.h>

export module makga.network.iocp.event;

import makga.network.session;

namespace makga::network {

export enum class IocpType
{
	ACCEPT,
	CONNECT,
	DISCONNECT,
	SEND,
	RECV
};

export class IocpObject; // forward declaration for use in IocpEvent

export class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent() = delete;
	IocpEvent(IocpType type)
		: type_(type), owner_(nullptr) {}
	IocpEvent(const IocpEvent&) = delete;
	IocpEvent(IocpEvent&&) = delete;
	IocpEvent& operator=(const IocpEvent&) = delete;
	IocpEvent& operator=(IocpEvent&&) = delete;
	virtual ~IocpEvent() = default;

	bool Initialize() { owner_ = nullptr; return true; }
	void Finalize() { owner_ = nullptr; }

	IocpType type_;
	std::shared_ptr<IocpObject> owner_; // dispatch
};

export class IocpAcceptEvent : public IocpEvent
{
public:
	IocpAcceptEvent() : IocpEvent(IocpType::ACCEPT) {}
	IocpAcceptEvent(const IocpAcceptEvent&) = delete;
	IocpAcceptEvent(IocpAcceptEvent&&) = delete;
	IocpAcceptEvent& operator=(const IocpAcceptEvent&) = delete;
	IocpAcceptEvent& operator=(IocpAcceptEvent&&) = delete;
	virtual ~IocpAcceptEvent() = default;

public:
	std::shared_ptr<NetSession> session_ = nullptr; // accept session
};

export class IocpConnectEvent : public IocpEvent
{
public:
	IocpConnectEvent() : IocpEvent(IocpType::CONNECT) {}
	IocpConnectEvent(const IocpConnectEvent&) = delete;
	IocpConnectEvent(IocpConnectEvent&&) = delete;
	IocpConnectEvent& operator=(const IocpConnectEvent&) = delete;
	IocpConnectEvent& operator=(IocpConnectEvent&&) = delete;
	virtual ~IocpConnectEvent() = default;

public:
	std::shared_ptr<NetSession> session_ = nullptr; // connect session
};

export class IocpDisconnectEvent : public IocpEvent
{
public:
	IocpDisconnectEvent() : IocpEvent(IocpType::DISCONNECT) {}
	IocpDisconnectEvent(const IocpDisconnectEvent&) = delete;
	IocpDisconnectEvent(IocpDisconnectEvent&&) = delete;
	IocpDisconnectEvent& operator=(const IocpDisconnectEvent&) = delete;
	IocpDisconnectEvent& operator=(IocpDisconnectEvent&&) = delete;
	virtual ~IocpDisconnectEvent() = default;
};

export class IocpRecvEvent : public IocpEvent
{
public:
	IocpRecvEvent() : IocpEvent(IocpType::RECV) {}
	IocpRecvEvent(const IocpRecvEvent&) = delete;
	IocpRecvEvent(IocpRecvEvent&&) = delete;
	IocpRecvEvent& operator=(const IocpRecvEvent&) = delete;
	IocpRecvEvent& operator=(IocpRecvEvent&&) = delete;
	virtual ~IocpRecvEvent() = default;
};

export template<typename T>
class IocpSendEvent : public IocpEvent
{
public:
	IocpSendEvent() : IocpEvent(IocpType::SEND) {}
	IocpSendEvent(const IocpSendEvent&) = delete;
	IocpSendEvent(IocpSendEvent&&) = delete;
	IocpSendEvent& operator=(const IocpSendEvent&) = delete;
	IocpSendEvent& operator=(IocpSendEvent&&) = delete;
	virtual ~IocpSendEvent() = default;

	std::vector<std::shared_ptr<T>> send_buffer_;
};

export class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
	IocpObject() = default;
	virtual ~IocpObject() = default;

	virtual void Dispatch(IocpEvent* iocp_event, int bytes_transferred = 0) = 0;
	virtual HANDLE GetHandle() const = 0;
};

} // namespace makga::network