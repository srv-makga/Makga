module;

export module makga.network.iouring.core;

import <cstddef>;
import <cstdint>;
import <memory>;

namespace makga::network {

export enum class IoUringOperation : std::uint8_t
{
    Nop,
    Accept,
    Receive,
    Send,
    Close,
};

export struct IoUringConfig
{
    // The kernel clamps this range. One producer submits through each IoUringCore instance.
    std::uint32_t queue_depth = 4096;
    bool clamp_queue_depth = true;
};

export struct IoUringCompletion
{
    std::uint64_t user_data = 0;
    std::int32_t result = 0;
    std::uint32_t flags = 0;
};

// Raw io_uring backend. Buffers passed to SubmitReceive/SubmitSend must remain valid
// until their completion is returned by PollCompletions/WaitCompletions.
export class IoUringCore
{
public:
    explicit IoUringCore(IoUringConfig config = {});
    ~IoUringCore();

    IoUringCore(const IoUringCore&) = delete;
    IoUringCore& operator=(const IoUringCore&) = delete;

    bool Initialize();
    void Finalize();

    bool IsSupported() const;
    bool IsInitialized() const;
    int GetRingFd() const;

    bool SubmitNop(std::uint64_t user_data);
    bool SubmitAccept(int listen_fd, std::uint32_t accept_flags, std::uint64_t user_data);
    bool SubmitReceive(int socket_fd, void* buffer, std::size_t length, std::uint32_t flags, std::uint64_t user_data);
    bool SubmitSend(int socket_fd, const void* buffer, std::size_t length, std::uint32_t flags, std::uint64_t user_data);
    bool SubmitClose(int fd, std::uint64_t user_data);

    // Pushes all queued SQEs to the kernel. Returns false only on an io_uring_enter failure.
    bool Submit();
    std::size_t PollCompletions(IoUringCompletion* output, std::size_t capacity);
    bool WaitCompletions(std::uint32_t minimum_completions, IoUringCompletion* output,
        std::size_t capacity, std::size_t& completed);

private:
    struct State;
    bool Prepare(IoUringOperation operation, int fd, const void* buffer, std::size_t length,
        std::uint32_t flags, std::uint64_t user_data);

    IoUringConfig config_;
    std::unique_ptr<State> state_;
};

} // namespace makga::network
