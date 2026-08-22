export module makga.network.iouring.acceptor;

import <atomic>;
import <cstddef>;
import <cstdint>;
import <functional>;
import <memory>;
import <mutex>;
import <thread>;
import <unordered_map>;

export import makga.network.acceptor;
export import makga.network.iouring.service;
export import makga.network.iouring.session;

namespace makga::network {

// io_uring server-side accept loop. The caller owns the listening fd until Start succeeds;
// thereafter this object closes it only through IORING_OP_CLOSE during Stop.
export class IoUringAcceptor final : public NetAcceptor
{
public:
    using SessionFactory = std::function<std::shared_ptr<IoUringSession>()>;
    using SessionRelease = std::function<void(std::shared_ptr<IoUringSession>)>;

    IoUringAcceptor(IoUringService* service, int listen_fd, std::size_t max_connections,
        SessionFactory session_factory, SessionRelease session_release = {});
    ~IoUringAcceptor() override;

    IoUringAcceptor(const IoUringAcceptor&) = delete;
    IoUringAcceptor& operator=(const IoUringAcceptor&) = delete;

    bool Start() override;
    void Stop() override;
    bool CanAcceptSession() const override;
    std::size_t GetConnectCount() const override;
    std::size_t GetMaxConnectCount() const override;

    // Dedicated lifecycle-sweeper entry point. It is safe to call independently from
    // the service completion worker and never invokes application callbacks under a lock.
    void SweepSessions();

private:
    struct State;
    bool SubmitAccept();
    void SweeperLoop();

    std::shared_ptr<State> state_;
    IoUringService::CompletionSubscriptionId completion_subscription_id_ = 0;
    std::thread sweeper_;
};

} // namespace makga::network
