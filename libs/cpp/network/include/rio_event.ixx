module;

#include <cstdint>
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
    explicit RioEvent(RioEventType type)
        : type_(type)
        , session_(nullptr)
    {
    }
    virtual ~RioEvent() = default;

    RioEventType type_;
    RioSession* session_;
};

export class RioRecvEvent final : public RioEvent
{
public:
    RioRecvEvent()
        : RioEvent(RioEventType::RECV)
    {
    }
};

export class RioSendEvent final : public RioEvent
{
public:
    RioSendEvent()
        : RioEvent(RioEventType::SEND)
    {
    }

    std::uint32_t slot_index_ = 0;
};
} // namespace makga::network
