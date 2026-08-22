module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
using SOCKET = int;
#endif

export module makga.network.iouring.session;

import <array>;
import <atomic>;
import <chrono>;
import <cstddef>;
import <cstdint>;
import <deque>;
import <memory>;
import <mutex>;
import <vector>;

export import makga.network.session;
export import makga.network.iouring.core;
export import makga.network.iouring.service;

namespace makga::network {

// High 8 bits identify the completion kind; the lower 56 bits carry a non-zero session id.
export enum class IoUringCompletionKind : std::uint8_t
{
    Accept = 1,
    Receive = 2,
    Send = 3,
    Close = 4,
};

// Linux fd-based NetSession adapter. A service/acceptor must retain a shared_ptr until
// IsIoLifecycleSafe() returns true; io_uring may still dereference submitted buffers.
export class IoUringSession : public NetSession
{
public:
    static constexpr std::size_t ReceiveBufferBytes = 8192;
    static constexpr std::size_t MaxSingleSendBytes = 8 * 1024 * 1024;
    static constexpr std::size_t MaxQueuedSendBytes = 16 * 1024 * 1024;
    static constexpr std::size_t MaxQueuedSends = 8192;

    enum class ShutdownMode : std::uint8_t { Active, Graceful, Forced };

    IoUringSession();
    ~IoUringSession() override;

    IoUringSession(const IoUringSession&) = delete;
    IoUringSession& operator=(const IoUringSession&) = delete;

    // SetSocket must be called before Initialize and transfers fd ownership to this session.
    void SetSocket(int socket_fd);
    bool Initialize(IoUringService* service);
    void Finalize();

    // Posts the first receive after OnConnect. Only one receive is in flight per session.
    bool StartReceive();
    bool Send(const char* buffer, std::size_t length);
    bool Send(std::shared_ptr<const std::vector<char>> payload);

    bool RequestGracefulClose(std::chrono::milliseconds timeout);
    void ForceClose();
    // Invoke from the owning lifecycle sweeper. A true result means it is safe to release
    // the final shared_ptr after Finalize() has been called.
    bool SweepIoLifecycle();
    bool IsIoLifecycleSafe() const;
    ShutdownMode GetShutdownMode() const;

    // Called only by the single IoUringService completion dispatcher after user-data routing.
    void HandleCompletion(IoUringOperation operation, const IoUringCompletion& completion);

    SOCKET GetSocket() const override;
    char* GetWritePosition() override;

    static std::uint64_t EncodeUserData(IoUringCompletionKind kind, Id session_id);
    static bool DecodeUserData(std::uint64_t user_data, IoUringCompletionKind& kind,
        Id& session_id);

private:
    bool PostReceiveLocked();
    bool PostNextSendLocked();
    bool BeginCloseWhenDrainedLocked();
    bool SubmitCloseLocked();
    void NotifyDisconnectOnce();

    IoUringService* service_ = nullptr;
    std::atomic<int> socket_fd_{-1};
    std::array<char, ReceiveBufferBytes> receive_buffer_{};

    mutable std::mutex mutex_;
    std::deque<std::shared_ptr<const std::vector<char>>> send_queue_;
    std::size_t send_offset_ = 0;
    std::size_t queued_send_bytes_ = 0;
    bool receive_in_flight_ = false;
    bool send_in_flight_ = false;
    bool close_in_flight_ = false;
    bool finalized_ = false;

    std::atomic<ShutdownMode> shutdown_mode_{ShutdownMode::Active};
    std::chrono::steady_clock::time_point graceful_deadline_{};
    std::atomic_bool disconnect_notified_{false};
};

} // namespace makga::network
