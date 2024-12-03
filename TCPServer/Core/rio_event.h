#pragma once
#include "io_context.hpp"

enum RioType
{
	SEND,
	RECV
};

namespace core {
namespace network {
class RioSession;
class RioEvent : public RIO_BUF
{
public:
	RioEvent() = delete;
	RioEvent(RioType _type) : m_type(_type) {}
	RioEvent(const RioEvent&) = delete;
	RioEvent(RioEvent&&) = delete;
	RioEvent& operator=(const RioEvent&) = delete;
	RioEvent& operator=(RioEvent&&) = delete;
	~RioEvent() = default;

	RioType m_type;
	std::shared_ptr<RioSession> m_owner;
};

class RioSendEvent : public RioEvent
{
public:
	RioSendEvent() : RioEvent(RioType::SEND) {}
	RioSendEvent(const RioSendEvent&) = delete;
	RioSendEvent(RioSendEvent&&) = delete;
	RioSendEvent& operator=(const RioSendEvent&) = delete;
	RioSendEvent& operator=(RioSendEvent&&) = delete;
	~RioSendEvent() = default;
};

class RioRecvEvent : public RioEvent
{
public:
	RioRecvEvent(RioType _type) : RioEvent(_type) {}
	RioRecvEvent(const RioSendEvent&) = delete;
	RioRecvEvent(RioSendEvent&&) = delete;
	RioRecvEvent& operator=(const RioSendEvent&) = delete;
	RioRecvEvent& operator=(RioSendEvent&&) = delete;
	~RioRecvEvent() = default;
};
} // namespace network
} // namespace core