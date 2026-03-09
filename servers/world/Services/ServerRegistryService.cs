using CSharp.World.Models;
using CSharp.World.Repository;
using System.Collections.Concurrent;

namespace CSharp.World.Services;

public class ServerRegistryService
{
    private readonly ConcurrentDictionary<string, GameServerInfo> _servers = new();
    private readonly WorldRepository _repository;
    private readonly ILogger<ServerRegistryService> _logger;
    private readonly IConfiguration _configuration;

    private TimeSpan HeartbeatTimeout => TimeSpan.FromSeconds(_configuration.GetValue("World:HeartbeatTimeoutSeconds", 30));

    public ServerRegistryService(WorldRepository repository, ILogger<ServerRegistryService> logger, IConfiguration configuration)
    {
        _repository = repository;
        _logger = logger;
        _configuration = configuration;
    }

    public async Task<bool> RegisterServerAsync(RegisterServerRequest request)
    {
        var server = new GameServerInfo
        {
            ServerId = request.ServerId,
            WorldId = request.WorldId,
            MapId = request.MapId,
            Host = request.Host,
            Port = request.Port,
            MaxPlayers = request.MaxPlayers,
            CurrentPlayers = 0,
            Status = ServerStatus.Active,
            LastHeartbeat = DateTime.UtcNow
        };

        _servers.AddOrUpdate(request.ServerId, server, (_, _) => server);
        await _repository.SetServerAsync(request.ServerId, server, HeartbeatTimeout);

        await RecalculateOnlineCountAsync(request.WorldId);

        _logger.LogInformation("Server registered: {ServerId} for world {WorldId}", request.ServerId, request.WorldId);
        return true;
    }

    public async Task<bool> HeartbeatAsync(string serverId)
    {
        if (_servers.TryGetValue(serverId, out var server))
        {
            server.LastHeartbeat = DateTime.UtcNow;
            await _repository.RefreshServerTtlAsync(serverId, HeartbeatTimeout);
            return true;
        }

        _logger.LogWarning("Heartbeat received for unknown server: {ServerId}", serverId);
        return false;
    }

    public async Task<bool> PlayerEnterAsync(string serverId, string mapId, long accountUid)
    {
        if (!_servers.TryGetValue(serverId, out var server))
        {
            _logger.LogWarning("Player enter for unknown server: {ServerId}", serverId);
            return false;
        }

        server.CurrentPlayers++;
        
        if (server.CurrentPlayers >= server.MaxPlayers)
        {
            server.Status = ServerStatus.Full;
        }

        await _repository.IncrPlayerCountAsync(serverId, mapId);
        await _repository.AddPlayerToMapAsync(serverId, mapId, accountUid);
        await _repository.IncrOnlineAsync(server.WorldId);

        _logger.LogInformation("Player {AccountUid} entered server {ServerId} map {MapId}. Current: {Current}/{Max}",
            accountUid, serverId, mapId, server.CurrentPlayers, server.MaxPlayers);

        return true;
    }

    public async Task<bool> PlayerLeaveAsync(string serverId, string mapId, long accountUid)
    {
        if (!_servers.TryGetValue(serverId, out var server))
        {
            _logger.LogWarning("Player leave for unknown server: {ServerId}", serverId);
            return false;
        }

        server.CurrentPlayers = Math.Max(0, server.CurrentPlayers - 1);
        
        if (server.CurrentPlayers < server.MaxPlayers)
        {
            server.Status = ServerStatus.Active;
        }

        await _repository.DecrPlayerCountAsync(serverId, mapId);
        await _repository.RemovePlayerFromMapAsync(serverId, mapId, accountUid);
        await _repository.DecrOnlineAsync(server.WorldId);

        _logger.LogInformation("Player {AccountUid} left server {ServerId} map {MapId}. Current: {Current}/{Max}",
            accountUid, serverId, mapId, server.CurrentPlayers, server.MaxPlayers);

        return true;
    }

    public async Task RemoveServerAsync(string serverId)
    {
        if (_servers.TryRemove(serverId, out var server))
        {
            await _repository.DeleteServerAsync(serverId);
            await RecalculateOnlineCountAsync(server.WorldId);

            _logger.LogInformation("Server removed: {ServerId} from world {WorldId}", serverId, server.WorldId);
        }
    }

    public IEnumerable<GameServerInfo> GetServersByWorld(string worldId)
    {
        return _servers.Values.Where(s => s.WorldId == worldId);
    }

    public GameServerInfo? GetServer(string serverId)
    {
        _servers.TryGetValue(serverId, out var server);
        return server;
    }

    public IEnumerable<GameServerInfo> GetAllServers()
    {
        return _servers.Values;
    }

    private async Task RecalculateOnlineCountAsync(string worldId)
    {
        var total = _servers.Values
            .Where(s => s.WorldId == worldId)
            .Sum(s => s.CurrentPlayers);

        await _repository.SetOnlineAsync(worldId, total);
    }
}
