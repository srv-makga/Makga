#pragma once

#include "io_context.hpp"

class RIOSession;

enum RIOType
{
	SEND,
	RECV
};

namespace core {
namespace network {
class RIOEvent : public RIO_BUF
{
public:
	RIOEvent() = delete;
	RIOEvent(RIOType _type) : m_type(_type) {}
	RIOEvent(const RIOEvent&) = delete;
	RIOEvent(RIOEvent&&) = delete;
	RIOEvent& operator=(const RIOEvent&) = delete;
	RIOEvent& operator=(RIOEvent&&) = delete;
	~RIOEvent() = default;

	RIOType m_type;
	std::shared_ptr<RIOSession> m_owner;
};

class RIOSendEvent : public RIOEvent
{
	RIOSendEvent(RIOType _type) : RIOEvent(_type) {}
	RIOSendEvent(const RIOSendEvent&) = delete;
	RIOSendEvent(RIOSendEvent&&) = delete;
	RIOSendEvent& operator=(const RIOSendEvent&) = delete;
	RIOSendEvent& operator=(RIOSendEvent&&) = delete;
	~RIOSendEvent() = default;
};

class RIORecvEvent : public RIOEvent
{
	RIORecvEvent(RIOType _type) : RIOEvent(_type) {}
	RIORecvEvent(const RIOSendEvent&) = delete;
	RIORecvEvent(RIOSendEvent&&) = delete;
	RIORecvEvent& operator=(const RIOSendEvent&) = delete;
	RIORecvEvent& operator=(RIOSendEvent&&) = delete;
	~RIORecvEvent() = default;
};
} // namespace network
} // namespace core