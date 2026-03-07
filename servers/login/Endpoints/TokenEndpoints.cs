using Login.Services;

namespace Login.Endpoints;

/// <summary>
/// JWT Access Token / Refresh Token 관리 엔드포인트.
/// </summary>
public static class TokenEndpoints
{
    public static void Map(RouteGroupBuilder group)
    {
        // POST /v1/auth/token/refresh
        // Body: { "refreshToken": "..." }
        //
        // 토큰 로테이션(Rotation) 처리:
        //   1. 기존 Refresh Token을 Redis에서 조회·검증
        //   2. 기존 토큰 즉시 삭제 (재사용 방지)
        //   3. 새 Refresh Token 발급 + Redis 저장
        //   4. 새 Access Token(JWT) 발급
        //
        // 성공: 200 OK → { access_token, refresh_token }
        // 실패: 400 필드 누락, 401 토큰 없음·만료
        group.MapPost("/token/refresh", async (
            TokenRequest req, RefreshTokenService refreshSvc, JwtService jwt) =>
        {
            if (string.IsNullOrWhiteSpace(req.RefreshToken))
                return Results.BadRequest(new { error = "refresh_token_required" });

            // Rotation: 기존 토큰 폐기 + 새 토큰 발급 (원자적이지 않으므로 극히 드물게 중복 발급 가능)
            var rotated = await refreshSvc.RotateAsync(req.RefreshToken);
            if (rotated is null) return Results.Unauthorized();

            var accessToken = jwt.IssueAccessToken(rotated.Value.AccountUid, rotated.Value.LoginType);
            return Results.Ok(new
            {
                access_token  = accessToken,
                refresh_token = rotated.Value.NewToken,
            });
        });

        // POST /v1/auth/token/revoke
        // Body: { "refreshToken": "..." }
        //
        // Redis에서 Refresh Token을 즉시 삭제한다.
        // 로그아웃 또는 보안 사고 시 토큰을 강제 무효화할 때 사용한다.
        // 성공: 200 OK (토큰이 이미 없어도 200 반환 — 멱등성 보장)
        group.MapPost("/token/revoke", async (TokenRequest req, RefreshTokenService refreshSvc) =>
        {
            if (string.IsNullOrWhiteSpace(req.RefreshToken))
                return Results.BadRequest(new { error = "refresh_token_required" });

            await refreshSvc.RevokeAsync(req.RefreshToken);
            return Results.Ok();
        });
    }
}

file record TokenRequest(string RefreshToken);
