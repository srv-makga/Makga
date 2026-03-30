module;

#include <memory>

export module makga.network.rio.event;

export enum class RioEventType
{
	ACCEPT,
	CONNECT,
	DISCONNECT,
	SEND,
	RECV
};

namespace makga::network {
export class RioSession;
export class RioEvent
{
public:
	RioEvent(RioEventType type)
		: type_(type)
		, session_(nullptr)
	{
	}
	virtual ~RioEvent() = default;

	RioEventType type_;
	RioSession* session_; // Context for the event
};

export class RioRecvEvent : public RioEvent
{
public:
	RioRecvEvent()
		: RioEvent(RioEventType::RECV)
	{
	}
};

export class RioSendEvent : public RioEvent
{
public:
	RioSendEvent() : RioEvent(RioEventType::SEND)
	{
	}
};
} // namespace makga::network