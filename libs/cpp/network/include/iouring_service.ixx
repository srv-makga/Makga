module;

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

export module makga.network.iouring.service;

export import makga.network.iouring.core;

namespace makga::network {

export struct IoUringServiceConfig
{
    IoUringConfig ring{};
    std::uint32_t poll_interval_ms = 1;
    std::size_t completion_batch_size = 256;
};

// A single-ring owner service. It serializes SQE producers and invokes completion
// handlers outside its submission lock. User buffers remain caller-owned until CQE.
export class IoUringService
{
public:
    using CompletionHandler = std::function<void(const IoUringCompletion&)>;
    using CompletionSubscriptionId = std::uint64_t;

    explicit IoUringService(IoUringServiceConfig config = {});
    ~IoUringService();

    IoUringService(const IoUringService&) = delete;
    IoUringService& operator=(const IoUringService&) = delete;

    bool Start();
    void Stop();
    bool IsRunning() const;
    bool IsSupported() const;

    // Legacy single handler. It remains independent from subscription handlers so
    // existing callers cannot overwrite an acceptor/client completion consumer.
    void SetCompletionHandler(CompletionHandler handler);
    CompletionSubscriptionId SubscribeCompletion(CompletionHandler handler);
    bool UnsubscribeCompletion(CompletionSubscriptionId subscription_id);

    bool SubmitAccept(int listen_fd, std::uint32_t accept_flags, std::uint64_t user_data);
    bool SubmitReceive(int socket_fd, void* buffer, std::size_t length,
        std::uint32_t flags, std::uint64_t user_data);
    bool SubmitSend(int socket_fd, const void* buffer, std::size_t length,
        std::uint32_t flags, std::uint64_t user_data);
    bool SubmitClose(int fd, std::uint64_t user_data);

    IoUringCore& GetCore();
    const IoUringCore& GetCore() const;

private:
    void WorkerLoop();

    IoUringServiceConfig config_;
    IoUringCore core_;
    std::atomic_bool running_{false};
    std::thread worker_;
    mutable std::mutex ring_mutex_;
    struct CompletionSubscription
    {
        CompletionSubscriptionId id = 0;
        CompletionHandler handler;
    };

    mutable std::mutex callback_mutex_;
    CompletionHandler legacy_completion_handler_;
    std::vector<CompletionSubscription> completion_subscriptions_;
    CompletionSubscriptionId next_completion_subscription_id_ = 1;
};

} // namespace makga::network
