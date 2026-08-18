module;

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

export module makga.network.rudp.session;

export import makga.network.endpoint;
export import makga.network.udp.socket;

namespace makga::network {

export enum class RudpState : std::uint8_t
{
    Idle, CookieWait, SynSent, SynReceived, Established, FinWait, Closed, TimedOut,
};

// All byte counts include only application payload; the wire header is excluded.
export struct RudpConfig
{
    static constexpr std::size_t MaxDatagramPayloadBytes = 1200;
    static constexpr std::size_t MaxMessageBytes = 8 * 1024 * 1024;
    static constexpr std::size_t MaxFragmentsPerMessage = 8192;
    // Packets currently handed to the congestion window / retransmit path.
    static constexpr std::size_t MaxOutstandingPackets = 1024;
    // Includes congestion-window waiting fragments of one logical message.
    static constexpr std::size_t MaxQueuedPackets = 8192;
    static constexpr std::size_t MaxReassemblyBytes = 16 * 1024 * 1024;
    static constexpr std::size_t MaxReorderPackets = 8192;

    std::uint32_t retransmit_ms = 200;
    std::uint32_t min_rto_ms = 50;
    std::uint32_t max_rto_ms = 2000;
    std::uint32_t idle_timeout_ms = 30000;
    std::uint32_t reassembly_timeout_ms = 5000;
    std::uint32_t cookie_rotation_ms = 60000;
    std::uint32_t max_handshakes_per_second = 2000;
    std::uint8_t max_retransmits = 8;
    bool require_cookie = true;
    bool enable_encryption = false;
    std::array<std::uint8_t, 32> encryption_key{};
};

export class RudpHost;

// RUDP composes UdpSocket because one UDP socket multiplexes many peers.
export class RudpSession : public std::enable_shared_from_this<RudpSession>
{
public:
    using OnMessage = std::function<void(const char* data, std::size_t size)>;
    using OnStateChanged = std::function<void(RudpState)>;

    ~RudpSession();
    RudpState State() const;
    bool IsEstablished() const;
    std::uint32_t ConnectionId() const;
    IPEndPoint Remote() const;
    void SetOnMessage(OnMessage callback);
    void SetOnStateChanged(OnStateChanged callback);
    bool StartClient();
    bool Send(const char* data, std::size_t size);
    void Close();

private:
    friend class RudpHost;

    struct PendingPacket
    {
        std::uint32_t sequence = 0;
        std::uint8_t flags = 0;
        std::uint32_t message_id = 0;
        std::uint16_t fragment_index = 0;
        std::uint16_t fragment_count = 0;
        std::uint32_t cookie = 0;
        std::vector<char> payload;
        std::uint64_t first_send_ms = 0;
        std::uint64_t last_send_ms = 0;
        std::uint8_t attempts = 0;
        bool sent = false;
        bool retransmitted = false;
    };

    struct ReassemblyMessage
    {
        std::uint16_t fragment_count = 0;
        std::size_t total_bytes = 0;
        std::uint64_t created_ms = 0;
        std::vector<std::vector<char>> fragments;
        std::vector<bool> received;
    };

    struct OrderedPacket
    {
        std::uint32_t message_id = 0;
        std::uint16_t fragment_index = 0;
        std::uint16_t fragment_count = 0;
        std::vector<char> payload;
    };

    RudpSession(RudpHost* host, IPEndPoint remote, std::uint32_t connection_id,
        bool client_side, RudpConfig config);
    bool OnDatagram(const char* data, std::size_t size, std::uint64_t now_ms);
    void Update(std::uint64_t now_ms);
    bool IsTerminal() const;

    bool SendFrame(std::uint8_t flags, std::uint32_t message_id,
        std::uint16_t fragment_index, std::uint16_t fragment_count,
        const char* data, std::size_t size, bool track_reliable, std::uint64_t now_ms);
    bool SendExisting(const PendingPacket& packet);
    void AckOutstanding(std::uint32_t ack, std::uint32_t ack_bits, std::uint64_t now_ms);
    bool RecordReceived(std::uint32_t sequence);
    bool InsertOrderedPayload(std::uint32_t sequence, OrderedPacket packet,
        std::vector<OrderedPacket>& ready_packets);
    bool InsertFragment(std::uint32_t message_id, std::uint16_t fragment_index,
        std::uint16_t fragment_count, const char* data, std::size_t size,
        std::uint64_t now_ms, std::vector<char>& complete_message);
    void ExpireReassembly(std::uint64_t now_ms);
    void UpdateRtt(std::uint64_t sample_ms);
    std::uint32_t CurrentRtoMs() const;
    void ChangeState(RudpState next_state, OnStateChanged& callback);

    RudpHost* host_;
    IPEndPoint remote_;
    const std::uint32_t connection_id_;
    const bool client_side_;
    const RudpConfig config_;
    mutable std::mutex mutex_;
    RudpState state_{RudpState::Idle};
    std::uint32_t next_sequence_{1};
    std::uint32_t next_message_id_{1};
    std::uint32_t latest_remote_sequence_{0};
    std::uint32_t received_history_{0};
    std::uint32_t next_delivery_sequence_{1};
    std::uint64_t last_receive_ms_{0};
    std::uint64_t smoothed_rtt_ms_{0};
    std::uint64_t rtt_variance_ms_{0};
    std::uint32_t congestion_window_{4};
    std::uint32_t slow_start_threshold_{64};
    std::uint32_t congestion_credit_{0};
    std::uint32_t pending_cookie_{0};
    bool encryption_active_{false};
    void* aes_algorithm_{nullptr};
    void* aes_key_{nullptr};
    std::vector<std::uint8_t> aes_key_object_;
    std::unordered_map<std::uint32_t, PendingPacket> pending_;
    std::deque<std::uint32_t> unsent_sequences_;
    std::uint64_t next_retransmit_scan_ms_{0};
    std::unordered_map<std::uint32_t, OrderedPacket> reorder_buffer_;
    std::unordered_map<std::uint32_t, ReassemblyMessage> reassembly_;
    std::size_t reassembly_bytes_{0};
    OnMessage on_message_;
    OnStateChanged on_state_changed_;
};

export class RudpHost : public std::enable_shared_from_this<RudpHost>
{
public:
    using OnAccepted = std::function<void(std::shared_ptr<RudpSession>)>;

    explicit RudpHost(std::shared_ptr<UdpSocket> udp_socket, RudpConfig config = {});
    ~RudpHost();
    RudpHost(const RudpHost&) = delete;
    RudpHost& operator=(const RudpHost&) = delete;

    bool Start();
    void Stop();
    bool IsRunning() const;
    std::shared_ptr<RudpSession> Connect(const IPEndPoint& remote);
    void SetOnAccepted(OnAccepted callback);
    void Update();
    std::size_t SessionCount() const;

private:
    friend class RudpSession;

    bool SendRaw(const IPEndPoint& remote, const char* data, std::size_t size);
    void OnUdpDatagram(const IPEndPoint& remote, const char* data, int size);
    void RemoveTerminalSessions();
    std::uint32_t MakeCookie(const IPEndPoint& remote, std::uint32_t connection_id,
        std::uint32_t epoch) const;
    bool ValidateCookie(const IPEndPoint& remote, std::uint32_t connection_id,
        std::uint32_t cookie, std::uint64_t now_ms) const;
    bool ConsumeHandshakeBudget(std::uint64_t now_ms);

    std::shared_ptr<UdpSocket> udp_socket_;
    const RudpConfig config_;
    std::atomic_bool running_{false};
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<RudpSession>> sessions_;
    OnAccepted on_accepted_;
    std::unordered_set<std::string> accepted_session_keys_;
    UdpSocket::MessageSubscriptionId udp_subscription_id_{0};
    std::array<std::uint8_t, 16> cookie_secret_{};
    bool cookie_secret_ready_{false};
    std::uint64_t handshake_window_ms_{0};
    std::uint32_t handshakes_in_window_{0};
    std::atomic_uint32_t next_connection_id_{1};
};

} // namespace makga::network
