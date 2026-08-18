module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#endif


export module makga.network.rio.session;

import <array>;
import <atomic>;
import <chrono>;
import <cstddef>;
import <cstdint>;
import <deque>;
import <memory>;
import <mutex>;
import <vector>;

import makga.network.session;
import makga.network.rio.core;
import makga.network.rio.event;
import makga.network.endpoint;
import makga.network.rio.service;

namespace makga::network {
export class RioSession : public NetSession
{
public:
    static constexpr std::size_t BufferSize = 8192;
    static constexpr std::size_t MaxOutstandingSends = 8192;
    static constexpr std::size_t MaxPendingSends = 10000;
    static constexpr std::size_t MaxPendingSendBytes = 64 * 1024 * 1024;
    static constexpr std::size_t MaxSingleSendBytes = 8 * 1024 * 1024;
    static constexpr std::size_t MaxInFlightSends = 64;

    RioSession();
    virtual ~RioSession();

    bool Initialize(RioService* service);
    void Finalize();
    // 연결이 성립된 뒤 최초 RIO receive를 등록하는 공개 진입점이다.
    bool StartReceive();

    enum class ShutdownMode : std::uint8_t { Active, Graceful, Forced };
    // 신규 수신·송신 수락을 중단하고 이미 수락된 송신과 실제 RIO 요청의 drain을 허용한다.
    bool RequestGracefulClose(std::chrono::milliseconds timeout);
    // 송신 대기열을 폐기하고 socket shutdown으로 진행 중인 RIO completion을 유도한다.
    void ForceClose();
    // 전용 lifecycle sweeper에서 호출한다. true면 Finalize를 실행해도 안전하다.
    bool SweepIoLifecycle();
    ShutdownMode GetShutdownMode() const { return shutdown_mode_.load(); }
    bool IsIoLifecycleSafe() const;

    virtual void OnRecv(int bytes_transferred) = 0;
    virtual void OnSend(int bytes_transferred) = 0;
    virtual void OnDisconnect() = 0;

    SOCKET GetSocket() const override { return socket_; }
    void SetSocket(SOCKET socket) { socket_ = socket; }
    char* GetWritePosition() override;

    // Legacy path: one copy into a shared payload. New callers should pass a
    // shared payload to avoid the queue-stage copy before registered-buffer copy.
    bool Send(char* buffer, int length);
    bool Send(std::shared_ptr<const std::vector<char>> payload);
    void HandleSendCompletion(int bytes_transferred, std::uint32_t slot_index);

protected:
    bool PostRecv();
    bool PostPendingSends(); // caller holds io_mutex_
private:
    struct PendingSend
    {
        std::shared_ptr<const std::vector<char>> payload;
        std::size_t offset = 0;
    };

    struct SendSlot
    {
        RioSendEvent event;
        bool in_flight = false;
        std::size_t bytes = 0;
    };

    RioService* service_ = nullptr;
    SOCKET socket_ = INVALID_SOCKET;
    RIO_RQ request_queue_ = RIO_INVALID_RQ;
    RIO_BUFFERID buffer_id_ = RIO_INVALID_BUFFERID;
    char* registered_memory_ = nullptr;
    char* recv_buffer_ = nullptr;

    std::deque<PendingSend> pending_sends_;
    std::size_t pending_send_bytes_ = 0;
    std::array<SendSlot, MaxInFlightSends> send_slots_{};
    std::size_t active_send_count_ = 0;
    std::atomic<ShutdownMode> shutdown_mode_{ShutdownMode::Active};
    std::atomic_uint64_t graceful_deadline_tick_{0};
    std::atomic_bool finalize_started_{false};

    mutable std::mutex io_mutex_;

    RioRecvEvent recv_event_;
};
} // namespace makga::network
