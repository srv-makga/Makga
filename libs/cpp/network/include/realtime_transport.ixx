module;

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

export module makga.network.realtime.transport;

export import makga.network.endpoint;
export import makga.network.iocp.core;
export import makga.network.udp.socket;
export import makga.network.rudp.session;

namespace makga::network {

export enum class GameTransportChannel : std::uint8_t
{
    TcpIocp,
    TcpRio,
    UdpLatest,
    RudpOrdered,
};

export enum class GameMessageClass : std::uint8_t
{
    Authentication, SessionControl, Inventory, Trade, Chat, ZoneTransfer,
    Position, Rotation, Camera, Snapshot,
    Input, Spawn, Despawn, Combat, Skill, EntityDelta,
};

export constexpr GameTransportChannel SelectGameTransport(GameMessageClass message)
{
    switch (message)
    {
    case GameMessageClass::Position:
    case GameMessageClass::Rotation:
    case GameMessageClass::Camera:
    case GameMessageClass::Snapshot: return GameTransportChannel::UdpLatest;
    case GameMessageClass::ZoneTransfer: return GameTransportChannel::TcpRio;
    case GameMessageClass::Input:
    case GameMessageClass::Spawn:
    case GameMessageClass::Despawn:
    case GameMessageClass::Combat:
    case GameMessageClass::Skill:
    case GameMessageClass::EntityDelta: return GameTransportChannel::RudpOrdered;
    default: return GameTransportChannel::TcpIocp;
    }
}

export struct RealtimeTransportConfig
{
    static constexpr std::size_t MaxUdpLatestMessageBytes = 8 * 1024 * 1024;
    static constexpr std::size_t MaxUdpLatestReassemblyBytes = 16 * 1024 * 1024;
    static constexpr std::uint32_t UdpLatestReassemblyTimeoutMs = 1000;
    // Servers use fixed ports; clients set enabled=true with port=0 for an ephemeral bind.
    bool enable_udp_latest = false;
    bool enable_rudp = false;
    std::uint16_t udp_port = 0;
    std::uint16_t rudp_port = 0;
    RudpConfig rudp{};
};

export class RealtimeTransportEndpoint
{
public:
    using DatagramHandler = std::function<void(const IPEndPoint&, const char*, std::size_t)>;

    RealtimeTransportEndpoint(std::shared_ptr<IocpCore> core, RealtimeTransportConfig config);
    ~RealtimeTransportEndpoint();
    RealtimeTransportEndpoint(const RealtimeTransportEndpoint&) = delete;
    RealtimeTransportEndpoint& operator=(const RealtimeTransportEndpoint&) = delete;

    bool Start();
    void Stop();
    void Update();
    bool IsRunning() const;

    void SetUdpLatestHandler(DatagramHandler handler);
    void SetRudpOrderedHandler(DatagramHandler handler);

    bool SendUdpLatest(const IPEndPoint& remote, const char* data, std::size_t size);
    std::shared_ptr<RudpSession> ConnectRudp(const IPEndPoint& remote);
    bool SendRudpOrdered(const IPEndPoint& remote, const char* data, std::size_t size);
    std::size_t RudpSessionCount() const;

private:
    struct UdpLatestReassembly
    {
        std::uint16_t fragment_count = 0;
        std::size_t total_bytes = 0;
        std::uint64_t created_ms = 0;
        std::vector<std::vector<char>> fragments;
        std::vector<bool> received;
    };

    void OnUdpLatestDatagram(const IPEndPoint& remote, const char* data, int size);
    void ExpireUdpLatestReassemblies(std::uint64_t now_ms);
    void OnRudpAccepted(std::shared_ptr<RudpSession> session);
    void BindRudpSession(const std::shared_ptr<RudpSession>& session);
    static std::string EndpointKey(const IPEndPoint& endpoint);

    std::shared_ptr<IocpCore> core_;
    const RealtimeTransportConfig config_;
    std::shared_ptr<UdpSocket> udp_socket_;
    std::shared_ptr<UdpSocket> rudp_socket_;
    std::shared_ptr<RudpHost> rudp_host_;
    std::atomic_bool running_{false};
    mutable std::mutex mutex_;
    DatagramHandler udp_latest_handler_;
    DatagramHandler rudp_ordered_handler_;
    std::atomic_uint32_t next_udp_latest_message_id_{1};
    std::unordered_map<std::string, UdpLatestReassembly> udp_latest_reassembly_;
    std::size_t udp_latest_reassembly_bytes_{0};
    std::unordered_map<std::string, std::shared_ptr<RudpSession>> rudp_sessions_;
};

} // namespace makga::network

