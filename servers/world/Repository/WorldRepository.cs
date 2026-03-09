using StackExchange.Redis;
using CSharp.World.Models;
using System.Text.Json;

namespace CSharp.World.Repository;

public class WorldRepository
{
    private readonly IConnectionMultiplexer _redis;
    private readonly IDatabase _db;

    public WorldRepository(IConnectionMultiplexer redis)
    {
        _redis = redis;
        _db = redis.GetDatabase();
    }

    public async Task SetServerAsync(string serverId, GameServerInfo server, TimeSpan ttl)
    {
        var key = $"server:{serverId}";
        var json = JsonSerializer.Serialize(server);
        await _db.StringSetAsync(key, json, ttl);
    }

    public async Task<GameServerInfo?> GetServerAsync(string serverId)
    {
        var key = $"server:{serverId}";
        var json = await _db.StringGetAsync(key);
        
        if (!json.HasValue)
            return null;

        return JsonSerializer.Deserialize<GameServerInfo>(json.ToString());
    }

    public async Task<bool> DeleteServerAsync(string serverId)
    {
        return await _db.KeyDeleteAsync($"server:{serverId}");
    }

    public async Task RefreshServerTtlAsync(string serverId, TimeSpan ttl)
    {
        await _db.KeyExpireAsync($"server:{serverId}", ttl);
    }

    public async Task<long> GetOnlineAsync(string worldId)
    {
        var value = await _db.StringGetAsync($"online:{worldId}");
        return value.HasValue ? (long)value : 0;
    }

    public async Task SetOnlineAsync(string worldId, long count)
    {
        await _db.StringSetAsync($"online:{worldId}", count);
    }

    public async Task IncrOnlineAsync(string worldId)
    {
        await _db.StringIncrementAsync($"online:{worldId}");
    }

    public async Task DecrOnlineAsync(string worldId)
    {
        var value = await _db.StringDecrementAsync($"online:{worldId}");
        if (value < 0)
        {
            await _db.StringSetAsync($"online:{worldId}", 0);
        }
    }

    public async Task<long> GetPlayerCountAsync(string serverId, string mapId)
    {
        var key = $"player-count:{serverId}:{mapId}";
        var value = await _db.StringGetAsync(key);
        return value.HasValue ? (long)value : 0;
    }

    public async Task IncrPlayerCountAsync(string serverId, string mapId)
    {
        await _db.StringIncrementAsync($"player-count:{serverId}:{mapId}");
    }

    public async Task DecrPlayerCountAsync(string serverId, string mapId)
    {
        var value = await _db.StringDecrementAsync($"player-count:{serverId}:{mapId}");
        if (value < 0)
        {
            await _db.StringSetAsync($"player-count:{serverId}:{mapId}", 0);
        }
    }

    public async Task AddPlayerToMapAsync(string serverId, string mapId, long accountUid)
    {
        var key = $"player-list:{serverId}:{mapId}";
        await _db.SetAddAsync(key, accountUid);
    }

    public async Task RemovePlayerFromMapAsync(string serverId, string mapId, long accountUid)
    {
        var key = $"player-list:{serverId}:{mapId}";
        await _db.SetRemoveAsync(key, accountUid);
    }

    public async Task<long> GetMapPlayerCountAsync(string serverId, string mapId)
    {
        var key = $"player-list:{serverId}:{mapId}";
        return await _db.SetLengthAsync(key);
    }

    public async Task<bool> IsPlayerInMapAsync(string serverId, string mapId, long accountUid)
    {
        var key = $"player-list:{serverId}:{mapId}";
        return await _db.SetContainsAsync(key, accountUid);
    }
}
