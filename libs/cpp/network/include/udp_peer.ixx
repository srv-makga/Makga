module;

#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <atomic>

export module makga.network.udp.peer;

export import makga.network.iocp.core;
export import makga.network.udp.socket;
export import makga.network.endpoint;

namespace makga::network {
export class UdpPeer
{
public:
    using OnMessageCb = std::function<void(const IPEndPoint& from, const char* data, int len)>;

    UdpPeer(std::shared_ptr<IocpCore> iocp_core);
    ~UdpPeer();

    bool Start(unsigned short local_port);
    void Stop();

    void SetOnMessage(OnMessageCb cb);

    bool JoinGroup(const std::string& group, const std::string& control_host, unsigned short control_port, unsigned short my_udp_port);
    bool LeaveGroup(const std::string& group, const std::string& control_host, unsigned short control_port, unsigned short my_udp_port);

    bool BroadcastToGroup(const std::string& group, const std::string& control_host, unsigned short control_port, const char* data, int len);
    bool SendToPeer(const std::string& ip, unsigned short port, const char* data, int len);

private:
    std::shared_ptr<IocpCore> iocp_core_;
    std::shared_ptr<UdpSocket> socket_;
    OnMessageCb on_message_;
    bool running_ = false;

    // message processing queue and worker thread to avoid blocking IOCP thread
    std::thread worker_thread_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    struct Msg { IPEndPoint from; std::vector<char> data; };
    std::deque<Msg> msg_queue_;
    std::atomic<bool> worker_running_{false};
};

} // namespace makga::network
