using Microsoft.AspNetCore.Routing;
using Ranking.Services;

namespace Ranking.Endpoints;

public static class UserRankEndpoints
{
    // GET /v1/ranking/{leaderboard}/user/{userId}
    public static void V1(RouteGroupBuilder group)
    {
        group.MapGet("/{leaderboard}/user/{userId}",async (
            string leaderboard,
            string userId,
            RankingService rankingService) =>
        {
            var entry = await rankingService.GetUserRankAsync(leaderboard, userId);
            return entry is null ? Results.NotFound() : Results.Ok(entry);
        });
    }
}
