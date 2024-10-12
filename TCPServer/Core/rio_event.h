#pragma once

#include "io_context.hpp"

class RIOSession;

namespace core {
namespace network {
class RIOEvent : public RIO_BUF
{
public:
	using Session_t = std::shared_ptr<RIOSession>;

	RIOEvent() = default;
	~RIOEvent() = default;

	Session_t m_owner;
};

class RIOSendEvent : public RIOEvent
{
	RIOSendEvent() : RIOEvent() {}
	RIOSendEvent(const RIOSendEvent&) = delete;
	RIOSendEvent(RIOSendEvent&&) = delete;
	RIOSendEvent& operator=(const RIOSendEvent&) = delete;
	RIOSendEvent& operator=(RIOSendEvent&&) = delete;
	~RIOSendEvent() = default;

	Session_t m_owner;
};

class RIORecvEvent : public RIOEvent
{
	RIORecvEvent() : RIOEvent() {}
	RIORecvEvent(const RIOSendEvent&) = delete;
	RIORecvEvent(RIOSendEvent&&) = delete;
	RIORecvEvent& operator=(const RIOSendEvent&) = delete;
	RIORecvEvent& operator=(RIOSendEvent&&) = delete;
	~RIORecvEvent() = default;

	Session_t m_owner;
};
} // namespace network
} // namespace core