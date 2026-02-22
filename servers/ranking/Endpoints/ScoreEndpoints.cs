using Microsoft.AspNetCore.Routing;
using Ranking.Services;

namespace Ranking.Endpoints;

public static class ScoreEndpoints
{
	// POST /v1/ranking/{leaderboard}/score
	// Body: { "userId": "player1", "score": 1500 }
	public static void V1(RouteGroupBuilder group)
	{
		group.MapPost("/{leaderboard}/score",async (
			string leaderboard,
			ScoreRequest request,
			RankingService rankingService) =>
		{
			await rankingService.SetScoreAsync(leaderboard, request.UserId, request.Score);
			return Results.Ok();
		});
	}
}

record ScoreRequest(string UserId, double Score);