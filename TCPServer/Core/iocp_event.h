#pragma once

#include "net_header.h"
#include <vector>
#include <memory>

enum IOCPType
{
	ACCEPT,
	CONNECT,
	DISCONNECT,
	SEND,
	RECV
};

class IOCPObject;
class IOCPSession;

namespace core {
namespace network {
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent() = delete;
	IocpEvent(IOCPType _type);
	IocpEvent(const IocpEvent&) = delete;
	IocpEvent(IocpEvent&& other) = delete;
	IocpEvent& operator=(const IocpEvent&) = delete;
	IocpEvent& operator=(IocpEvent&&) = delete;
	virtual ~IocpEvent() = default;

	bool Initialize();
	void Finalize();

	IOCPType m_type;
	std::shared_ptr<IOCPObject> m_owner;
};

class IocpAcceptEvent : public IocpEvent
{
public:
	IocpAcceptEvent() = delete;
	IocpAcceptEvent(IOCPType _type);
	IocpAcceptEvent(const IocpAcceptEvent&) = delete;
	IocpAcceptEvent(IocpAcceptEvent&& other) = delete;
	IocpAcceptEvent& operator=(const IocpAcceptEvent&) = delete;
	IocpAcceptEvent& operator=(IocpAcceptEvent&&) = delete;
	virtual ~IocpAcceptEvent() = default;

private:
	std::shared_ptr<IOCPSession> m_session = nullptr; // accept된 session
};

class IocpConnectEvent : public IocpEvent
{
public:
	IocpConnectEvent() : IocpEvent(IOCPType::CONNECT) {}
	IocpConnectEvent(const IocpConnectEvent&) = delete;
	IocpConnectEvent(IocpConnectEvent&&) = delete;
	IocpConnectEvent& operator=(const IocpConnectEvent&) = delete;
	IocpConnectEvent& operator=(IocpConnectEvent&&) = delete;
	virtual ~IocpConnectEvent() = default;
};

class IocpDisconnectEvent : public IocpEvent
{
public:
	IocpDisconnectEvent() : IocpEvent(IOCPType::DISCONNECT) {}
	IocpDisconnectEvent(const IocpDisconnectEvent&) = delete;
	IocpDisconnectEvent(IocpDisconnectEvent&&) = delete;
	IocpDisconnectEvent& operator=(const IocpDisconnectEvent&) = delete;
	IocpDisconnectEvent& operator=(IocpDisconnectEvent&&) = delete;
	virtual ~IocpDisconnectEvent() = default;
};

class IocpRecvEvent : public IocpEvent
{
public:
	IocpRecvEvent() : IocpEvent(IOCPType::RECV) {}
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
	IocpSendEvent() : IocpEvent(IOCPType::SEND) {}
	IocpSendEvent(const IocpSendEvent&) = delete;
	IocpSendEvent(IocpSendEvent&&) = delete;
	IocpSendEvent& operator=(const IocpSendEvent&) = delete;
	IocpSendEvent& operator=(IocpSendEvent&&) = delete;
	virtual ~IocpSendEvent() = default;

	std::vector<std::shared_ptr<T>> m_send_buffer; // T는 전송될 데이터가 있는 네트워크용 버퍼
};
} // namespace network
} // namespace core