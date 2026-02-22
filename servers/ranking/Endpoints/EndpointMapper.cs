using Microsoft.AspNetCore.Builder;

namespace Ranking.Endpoints;

// 모든 엔드포인트 등록을 한 곳에서 관리.
// 버전 전환은 각 기능의 호출 메서드(V1, V2...)만 교체하면 됩니다. URL은 변하지 않습니다.
public static class EndpointMapper
{
    public static void MapAll(WebApplication app)
    {
        var topCacheTTL = int.TryParse(
            app.Configuration["RankingCache:TopExpirationSeconds"], out var s) ? s : 10;

        app.MapGet("/version", () => "1.0.0");
        app.MapGet("/ping", () => "pong");
        app.MapGet("/health", () => Results.Ok(new { status = "healthy" }));

        var ranking = app.MapGroup("/ranking");
        ScoreEndpoints.V1(ranking);
        TopEndpoints.V1(ranking, topCacheTTL);
        UserRankEndpoints.V1(ranking);
        RemoveEndpoints.V1(ranking);
    }
}
