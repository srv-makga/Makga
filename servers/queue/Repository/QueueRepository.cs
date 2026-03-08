using StackExchange.Redis;
using CSharp.Queue.Models;
using System.Text.Json;

namespace CSharp.Queue.Repository;

public class QueueRepository
{
    private readonly IConnectionMultiplexer _redis;
    private readonly IDatabase _db;

    public QueueRepository(IConnectionMultiplexer redis)
    {
        _redis = redis;
        _db = redis.GetDatabase();
    }

    // Capacity & Online
    public async Task<long> GetCapacityAsync(string worldId)
    {
        var value = await _db.StringGetAsync($"capacity:{worldId}");
        return value.HasValue ? (long)value : 0;
    }

    public async Task<long> GetOnlineAsync(string worldId)
    {
        var value = await _db.StringGetAsync($"online:{worldId}");
        return value.HasValue ? (long)value : 0;
    }

    public async Task SetCapacityAsync(string worldId, long capacity)
    {
        await _db.StringSetAsync($"capacity:{worldId}", capacity);
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

    // Queue (ZSET)
    public async Task<bool> AddToQueueAsync(string worldId, string ticketId, double timestamp)
    {
        return await _db.SortedSetAddAsync($"queue:{worldId}", ticketId, timestamp);
    }

    public async Task<SortedSetEntry[]> PopFromQueueAsync(string worldId, long count)
    {
        return await _db.SortedSetPopAsync($"queue:{worldId}", count, Order.Ascending);
    }

    public async Task<long> GetQueuePositionAsync(string worldId, string ticketId)
    {
        var rank = await _db.SortedSetRankAsync($"queue:{worldId}", ticketId, Order.Ascending);
        return rank.HasValue ? rank.Value : -1;
    }

    public async Task<long> GetQueueLengthAsync(string worldId)
    {
        return await _db.SortedSetLengthAsync($"queue:{worldId}");
    }

    public async Task<bool> RemoveFromQueueAsync(string worldId, string ticketId)
    {
        return await _db.SortedSetRemoveAsync($"queue:{worldId}", ticketId);
    }

    // Ticket (Hash)
    public async Task<bool> SetTicketAsync(QueueTicket ticket, TimeSpan ttl)
    {
        var key = $"ticket:{ticket.TicketId}";
        var entries = new HashEntry[]
        {
            new("accountUid", ticket.AccountUid),
            new("worldId", ticket.WorldId),
            new("status", ticket.Status.ToString()),
            new("createdAt", ticket.CreatedAt.ToString("o")),
            new("readyAt", ticket.ReadyAt?.ToString("o") ?? ""),
            new("serverInfo", ticket.ServerInfo != null ? JsonSerializer.Serialize(ticket.ServerInfo) : "")
        };

        await _db.HashSetAsync(key, entries);
        await _db.KeyExpireAsync(key, ttl);
        return true;
    }

    public async Task<QueueTicket?> GetTicketAsync(string ticketId)
    {
        var key = $"ticket:{ticketId}";
        var entries = await _db.HashGetAllAsync(key);
        
        if (entries.Length == 0)
            return null;

        var dict = entries.ToDictionary(
            e => e.Name.ToString(),
            e => e.Value.ToString()
        );

        var ticket = new QueueTicket
        {
            TicketId = ticketId,
            AccountUid = long.Parse(dict.GetValueOrDefault("accountUid", "0")),
            WorldId = dict.GetValueOrDefault("worldId", ""),
            Status = Enum.Parse<QueueStatus>(dict.GetValueOrDefault("status", "Waiting")),
            CreatedAt = DateTime.Parse(dict.GetValueOrDefault("createdAt", DateTime.UtcNow.ToString("o"))),
        };

        if (dict.ContainsKey("readyAt") && !string.IsNullOrEmpty(dict["readyAt"]))
        {
            ticket.ReadyAt = DateTime.Parse(dict["readyAt"]);
        }

        if (dict.ContainsKey("serverInfo") && !string.IsNullOrEmpty(dict["serverInfo"]))
        {
            ticket.ServerInfo = JsonSerializer.Deserialize<ServerInfo>(dict["serverInfo"]);
        }

        return ticket;
    }

    public async Task<bool> UpdateTicketStatusAsync(string ticketId, QueueStatus status, DateTime? readyAt = null, ServerInfo? serverInfo = null, TimeSpan? ttl = null)
    {
        var key = $"ticket:{ticketId}";
        
        var updates = new List<HashEntry>
        {
            new("status", status.ToString())
        };

        if (readyAt.HasValue)
        {
            updates.Add(new HashEntry("readyAt", readyAt.Value.ToString("o")));
        }

        if (serverInfo != null)
        {
            updates.Add(new HashEntry("serverInfo", JsonSerializer.Serialize(serverInfo)));
        }

        await _db.HashSetAsync(key, updates.ToArray());

        if (ttl.HasValue)
        {
            await _db.KeyExpireAsync(key, ttl.Value);
        }

        return true;
    }

    public async Task<bool> DeleteTicketAsync(string ticketId)
    {
        return await _db.KeyDeleteAsync($"ticket:{ticketId}");
    }

    // Account Queue Mapping (중복 입장 방지)
    public async Task<string?> GetAccountQueueAsync(long accountUid)
    {
        var value = await _db.StringGetAsync($"account:queue:{accountUid}");
        return value.HasValue ? value.ToString() : null;
    }

    public async Task SetAccountQueueAsync(long accountUid, string ticketId, TimeSpan ttl)
    {
        await _db.StringSetAsync($"account:queue:{accountUid}", ticketId, ttl);
    }

    public async Task DeleteAccountQueueAsync(long accountUid)
    {
        await _db.KeyDeleteAsync($"account:queue:{accountUid}");
    }

    // Entry Token
    public async Task<bool> SetEntryTokenAsync(string entryToken, EntryTokenInfo info, TimeSpan ttl)
    {
        var key = $"entry:{entryToken}";
        var entries = new HashEntry[]
        {
            new("accountUid", info.AccountUid),
            new("worldId", info.WorldId),
            new("serverId", info.ServerId)
        };

        await _db.HashSetAsync(key, entries);
        await _db.KeyExpireAsync(key, ttl);
        return true;
    }

    public async Task<EntryTokenInfo?> GetEntryTokenAsync(string entryToken)
    {
        var key = $"entry:{entryToken}";
        var entries = await _db.HashGetAllAsync(key);
        
        if (entries.Length == 0)
            return null;

        var dict = entries.ToDictionary(
            e => e.Name.ToString(),
            e => e.Value.ToString()
        );

        return new EntryTokenInfo
        {
            AccountUid = long.Parse(dict.GetValueOrDefault("accountUid", "0")),
            WorldId = dict.GetValueOrDefault("worldId", ""),
            ServerId = dict.GetValueOrDefault("serverId", "")
        };
    }

    public async Task<bool> DeleteEntryTokenAsync(string entryToken)
    {
        return await _db.KeyDeleteAsync($"entry:{entryToken}");
    }

    // Get all active world IDs
    public async Task<string[]> GetAllWorldIdsAsync()
    {
        var server = _redis.GetServer(_redis.GetEndPoints().First());
        var keys = server.Keys(pattern: "capacity:*").ToArray();
        
        return keys.Select(k => k.ToString().Replace("capacity:", "")).ToArray();
    }
}
