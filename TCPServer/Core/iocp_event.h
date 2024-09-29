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
class IOCPEvent : public OVERLAPPED
{
public:
	IOCPEvent() = delete;
	IOCPEvent(IOCPType _type);
	IOCPEvent(const IOCPEvent&) = delete;
	IOCPEvent(IOCPEvent&& other) = delete;
	IOCPEvent& operator=(const IOCPEvent&) = delete;
	IOCPEvent& operator=(IOCPEvent&&) = delete;
	virtual ~IOCPEvent() = default;

	void Initialize();
	void Finalize();

	IOCPType m_type;
	std::shared_ptr<IOCPObject> m_owner;
};

class IOCPAcceptEvent : public IOCPEvent
{
public:
	IOCPAcceptEvent() = delete;
	IOCPAcceptEvent(IOCPType _type);
	IOCPAcceptEvent(const IOCPAcceptEvent&) = delete;
	IOCPAcceptEvent(IOCPAcceptEvent&& other) = delete;
	IOCPAcceptEvent& operator=(const IOCPAcceptEvent&) = delete;
	IOCPAcceptEvent& operator=(IOCPAcceptEvent&&) = delete;
	virtual ~IOCPAcceptEvent() = default;

private:
	std::shared_ptr<IOCPSession> m_session = nullptr; // accept된 session
};

class IOCPConnectEvent : public IOCPEvent
{
public:
	IOCPConnectEvent() : IOCPEvent(IOCPType::CONNECT) {}
	IOCPConnectEvent(const IOCPConnectEvent&) = delete;
	IOCPConnectEvent(IOCPConnectEvent&&) = delete;
	IOCPConnectEvent& operator=(const IOCPConnectEvent&) = delete;
	IOCPConnectEvent& operator=(IOCPConnectEvent&&) = delete;
	virtual ~IOCPConnectEvent() = default;
};

class IOCPDisconnectEvent : public IOCPEvent
{
public:
	IOCPDisconnectEvent() : IOCPEvent(IOCPType::DISCONNECT) {}
	IOCPDisconnectEvent(const IOCPDisconnectEvent&) = delete;
	IOCPDisconnectEvent(IOCPDisconnectEvent&&) = delete;
	IOCPDisconnectEvent& operator=(const IOCPDisconnectEvent&) = delete;
	IOCPDisconnectEvent& operator=(IOCPDisconnectEvent&&) = delete;
	virtual ~IOCPDisconnectEvent() = default;
};

class IOCPRecvEvent : public IOCPEvent
{
public:
	IOCPRecvEvent() : IOCPEvent(IOCPType::RECV) {}
	IOCPRecvEvent(const IOCPRecvEvent&) = delete;
	IOCPRecvEvent(IOCPRecvEvent&&) = delete;
	IOCPRecvEvent& operator=(const IOCPRecvEvent&) = delete;
	IOCPRecvEvent& operator=(IOCPRecvEvent&&) = delete;
	virtual ~IOCPRecvEvent() = default;
};

template<typename T>
class IOCPSendEvent : public IOCPEvent
{
public:
	IOCPSendEvent() : IOCPEvent(IOCPType::SEND) {}
	IOCPSendEvent(const IOCPSendEvent&) = delete;
	IOCPSendEvent(IOCPSendEvent&&) = delete;
	IOCPSendEvent& operator=(const IOCPSendEvent&) = delete;
	IOCPSendEvent& operator=(IOCPSendEvent&&) = delete;
	virtual ~IOCPSendEvent() = default;

	std::vector<std::shared_ptr<T>> m_send_buffer; // T는 전송될 데이터가 있는 네트워크용 버퍼
};
} // namespace network
} // namespace core