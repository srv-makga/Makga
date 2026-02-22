using StackExchange.Redis;

namespace Ranking.Services;

public sealed class RankingService
{
    private readonly IDatabase _db;

    public RankingService(IConnectionMultiplexer redis)
    {
        _db = redis.GetDatabase();
    }

    // Sorted set key per leaderboard
    private static string Key(string leaderboard) => $"ranking:{leaderboard}";

    // Add or update a user's score (ZADD)
    public Task SetScoreAsync(string leaderboard, string userId, double score)
        => _db.SortedSetAddAsync(Key(leaderboard), userId, score);

    // Get top N entries by descending score (ZREVRANGE WITHSCORES)
    public async Task<IReadOnlyList<RankEntry>> GetTopAsync(string leaderboard, int count)
    {
        var entries = await _db.SortedSetRangeByRankWithScoresAsync(
            Key(leaderboard), 0, count - 1, Order.Descending);

        return entries.Select((e, i) => new RankEntry(
            Rank: i + 1,
            UserId: e.Element.ToString(),
            Score: e.Score)).ToList();
    }

    // Get a specific user's rank and score (ZREVRANK + ZSCORE)
    public async Task<RankEntry?> GetUserRankAsync(string leaderboard, string userId, bool useHighestRank = true)
    {
        var key = Key(leaderboard);

        var rankTask = _db.SortedSetRankAsync(key, userId, Order.Descending);
        var scoreTask = _db.SortedSetScoreAsync(key, userId);

        await Task.WhenAll(rankTask, scoreTask);

        long? rank = await rankTask;
        double? score = await scoreTask;

        if (rank is null || score is null)
            return null;

        if (false == useHighestRank)
        {
            // 동점자 중 최하위 순위 반환: 동점인 유저 수만큼 rank를 뒤로 밀어냄
            long tiedCount = await _db.SortedSetLengthAsync(key, score.Value, score.Value);
            rank = rank.Value + tiedCount - 1;
        }

        return new RankEntry(Rank: (int)rank.Value + 1, UserId: userId, Score: score.Value);
    }

    // Remove a user from the leaderboard (ZREM)
    public Task RemoveUserAsync(string leaderboard, string userId)
        => _db.SortedSetRemoveAsync(Key(leaderboard), userId);
}

public record RankEntry(int Rank, string UserId, double Score);
