using StackExchange.Redis;
using AdminTool.Models;

namespace AdminTool.Services;

public class RedisService
{
    private readonly RedisConfig _cfg;
    private readonly Lazy<ConnectionMultiplexer> _mux;

    public RedisService(MakgaConfig config)
    {
        _cfg = config.Redis;
        _mux = new Lazy<ConnectionMultiplexer>(() =>
        {
            var opts = new ConfigurationOptions
            {
                EndPoints = { { _cfg.Host, _cfg.Port } },
                DefaultDatabase = _cfg.Db,
                ConnectTimeout = 2000,
                SyncTimeout = 3000,
                AbortOnConnectFail = false,
            };
            if (!string.IsNullOrEmpty(_cfg.Password))
                opts.Password = _cfg.Password;
            return ConnectionMultiplexer.Connect(opts);
        });
    }

    private IDatabase Db => _mux.Value.GetDatabase();

    public async Task<bool> PingAsync()
    {
        try { await Db.PingAsync(); return true; }
        catch { return false; }
    }

    public async Task<List<string>> ScanKeysAsync(string pattern = "*", int count = 200)
    {
        var server = _mux.Value.GetServer(_mux.Value.GetEndPoints().First());
        var keys = new List<string>();
        await foreach (var key in server.KeysAsync(pattern: pattern, pageSize: count))
            keys.Add(key.ToString());
        return keys.Take(count).ToList();
    }

    public async Task<RedisKeyInfo?> GetKeyInfoAsync(string key)
    {
        var type = await Db.KeyTypeAsync(key);
        if (type == RedisType.None) return null;

        var ttl = await Db.KeyTimeToLiveAsync(key);
        var info = new RedisKeyInfo
        {
            Key = key,
            Type = type.ToString().ToLower(),
            Ttl = (long)(ttl?.TotalSeconds ?? -1),
        };

        switch (type)
        {
            case RedisType.String:
                info.StringValue = await Db.StringGetAsync(key);
                break;
            case RedisType.Hash:
                var hash = await Db.HashGetAllAsync(key);
                info.HashValue = hash.ToDictionary(e => e.Name.ToString(), e => e.Value.ToString());
                break;
            case RedisType.List:
                var list = await Db.ListRangeAsync(key, 0, 99);
                info.ListValue = list.Select(v => v.ToString()).ToList();
                break;
            case RedisType.Set:
                var set = await Db.SetMembersAsync(key);
                info.SetValue = set.Select(v => v.ToString()).ToList();
                break;
            case RedisType.SortedSet:
                var zset = await Db.SortedSetRangeByRankWithScoresAsync(key, 0, 99);
                info.ZSetValue = zset.Select(e => new ZSetEntry
                    { Member = e.Element.ToString(), Score = e.Score }).ToList();
                break;
        }
        return info;
    }

    public async Task SetStringAsync(string key, string value, long? ttlSeconds = null)
    {
        var expiry = ttlSeconds > 0 ? TimeSpan.FromSeconds(ttlSeconds.Value) : (TimeSpan?)null;
        await Db.StringSetAsync(key, value, expiry);
    }

    public async Task DeleteKeyAsync(string key) => await Db.KeyDeleteAsync(key);

    public async Task<string> GetServerSummaryAsync()
    {
        try
        {
            var server = _mux.Value.GetServer(_mux.Value.GetEndPoints().First());
            var info = await server.InfoAsync();
            var dict = info.SelectMany(g => g).ToDictionary(e => e.Key, e => e.Value);
            return $"v{dict.GetValueOrDefault("redis_version", "?")} | " +
                   $"keys:{dict.GetValueOrDefault("db0", "0")} | " +
                   $"mem:{dict.GetValueOrDefault("used_memory_human", "?")}";
        }
        catch { return "연결 실패"; }
    }

    public async Task FlushDbAsync() => await Db.ExecuteAsync("FLUSHDB");
}
