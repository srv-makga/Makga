module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif

export module makga.network.udp.socket;

import <atomic>;
import <cstdint>;
import <functional>;
import <memory>;
import <mutex>;
import <unordered_map>;
import <vector>;

export import makga.network.iocp.core;
export import makga.network.iocp.event;
export import makga.network.iocp.event;
export import makga.network.endpoint;

namespace makga::network {
export class UdpSocket : public IocpObject
{
public:
    using MessageSubscriptionId = std::uint64_t;
    using OnMessage = std::function<void(const IPEndPoint&, const char*, int)>;

    UdpSocket(std::shared_ptr<IocpCore> iocp_core);
    virtual ~UdpSocket();

    bool Open(unsigned short port);
    void Close();

    bool PostRecv(); // post an overlapped WSARecvFrom
    bool SendTo(const IPEndPoint& remote, const char* data, int len);

    // Legacy primary consumer. It is replaced atomically and coexists with subscriptions.
    void SetOnMessage(OnMessage cb);
    // Completion threads copy callbacks under this lock, then invoke them outside the lock.
    MessageSubscriptionId SubscribeOnMessage(OnMessage cb);
    bool UnsubscribeOnMessage(MessageSubscriptionId subscription_id);

    // IocpObject
    void Dispatch(IocpEvent* iocp_event, int bytes_transferred = 0) override;
    HANDLE GetHandle() const override;

private:
    std::shared_ptr<IocpCore> iocp_core_;
    SOCKET socket_;
    std::atomic_bool closing_{true};
    mutable std::mutex state_mutex_;
    mutable std::mutex callback_mutex_;
    OnMessage on_message_cb_;
    std::unordered_map<MessageSubscriptionId, OnMessage> message_subscribers_;
    MessageSubscriptionId next_subscription_id_{1};
};
} // namespace makga::network
