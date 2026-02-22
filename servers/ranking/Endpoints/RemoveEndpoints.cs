using Microsoft.AspNetCore.Routing;
using Ranking.Services;

namespace Ranking.Endpoints;

public static class RemoveEndpoints
{
    // DELETE /v1/ranking/{leaderboard}/user/{userId}
    public static void V1(RouteGroupBuilder group)
    {
        group.MapDelete("/{leaderboard}/user/{userId}",async (
            string leaderboard,
            string userId,
            RankingService rankingService) =>
        {
            await rankingService.RemoveUserAsync(leaderboard, userId);
            return Results.Ok();
        });
    }
}
