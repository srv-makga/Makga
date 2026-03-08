using CSharp.Queue.Models;
using StackExchange.Redis;
using System.Text.Json;

namespace CSharp.Queue.Services.ServerResolver;

public class DirectServerResolver : IWorldServerResolver
{
    private readonly IConnectionMultiplexer _redis;
    private readonly IDatabase _db;
    private readonly ILogger<DirectServerResolver> _logger;
    private int _roundRobinIndex = 0;

    public DirectServerResolver(IConnectionMultiplexer redis, ILogger<DirectServerResolver> logger)
    {
        _redis = redis;
        _db = redis.GetDatabase();
        _logger = logger;
    }

    public async Task<ServerInfo?> ResolveAsync(string worldId)
    {
        try
        {
            var key = $"servers:{worldId}";
            var length = await _db.ListLengthAsync(key);
            
            if (length == 0)
            {
                _logger.LogWarning("No servers found for worldId: {WorldId}", worldId);
                return null;
            }

            var index = Interlocked.Increment(ref _roundRobinIndex) % (int)length;
            var serverJson = await _db.ListGetByIndexAsync(key, index);
            
            if (!serverJson.HasValue)
            {
                return null;
            }

            return JsonSerializer.Deserialize<ServerInfo>(serverJson.ToString());
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to resolve server for worldId: {WorldId}", worldId);
            return null;
        }
    }
}
