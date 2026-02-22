using Microsoft.AspNetCore.Routing;
using Ranking.Services;

namespace Ranking.Endpoints;

public static class TopEndpoints
{
    // GET /v1/ranking/{leaderboard}/top?count=10
    public static void V1(RouteGroupBuilder group, int topCacheTTL)
    {
        group.MapGet("/{leaderboard}/top", async (
            string leaderboard,
            RankingService rankingService,
            int count = 10) =>
        {
            var entries = await rankingService.GetTopAsync(leaderboard, count);
            return Results.Ok(entries);
        }).CacheOutput(p => p.Expire(TimeSpan.FromSeconds(topCacheTTL))
                             .SetVaryByRouteValue("leaderboard")
                             .SetVaryByQuery("count"));
    }
}
