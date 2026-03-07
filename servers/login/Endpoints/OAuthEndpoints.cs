using Login.Services;

namespace Login.Endpoints;

/// <summary>
/// Google / Apple OAuth 로그인 엔드포인트.
///
/// 클라이언트 흐름:
///   1. 클라이언트가 각 플랫폼 SDK로 로그인 → ID Token 발급
///   2. ID Token을 이 서버로 전달
///   3. 서버가 플랫폼 공개키로 서명 검증 → 계정 조회/생성 → JWT 발급
///
/// 공통 오류 응답:
///   400 Bad Request  — idToken 필드 누락
///   401 Unauthorized — 플랫폼 토큰 검증 실패 (만료, 서명 오류 등)
///   403 Forbidden    — 계정 정지·비활성 상태
/// </summary>
public static class OAuthEndpoints
{
    public static void Map(RouteGroupBuilder group)
    {
        // ─────────────────────────────────────────────────────────────────
        // POST /v1/auth/google/login
        // Body: { "idToken": "<Google ID Token>" }
        //
        // 처리 흐름:
        //   1. GoogleAuthService.ValidateAsync() — Google 공개키로 서명 검증
        //      → payload.Subject (Google 고유 사용자 ID, 영구 불변)
        //      → payload.Email (Google 이메일)
        //   2. AccountService.LoginOAuthAsync("google", sub, email)
        //      → DB sp_upsert_oauth: 계정 조회 또는 신규 생성
        //   3. JWT Access Token + Refresh Token 발급
        // ─────────────────────────────────────────────────────────────────
        group.MapPost("/google/login", async (
            OAuthRequest req, GoogleAuthService googleAuth, AccountService svc) =>
        {
            if (string.IsNullOrWhiteSpace(req.IdToken))
                return Results.BadRequest(new { error = "id_token_required" });

            // Google ID Token 서명 검증 (Google.Apis.Auth 라이브러리)
            var identity = await googleAuth.ValidateAsync(req.IdToken);
            if (identity is null) return Results.Unauthorized();

            var (response, error) = await svc.LoginOAuthAsync(
                "google", identity.Value.Sub, identity.Value.Email);

            return error is null ? Results.Ok(response) : Results.StatusCode(403);
        });

        // ─────────────────────────────────────────────────────────────────
        // POST /v1/auth/apple/login
        // Body: { "idToken": "<Apple ID Token>" }
        //
        // 처리 흐름:
        //   1. AppleAuthService.ValidateAsync() — Apple JWKS로 서명 검증
        //      → claims["sub"] (Apple 고유 사용자 ID, 팀 기준 영구 불변)
        //      → claims["email"] (실제 이메일 또는 Apple 중계 주소)
        //   2. AccountService.LoginOAuthAsync("apple", sub, email)
        //      → DB sp_upsert_oauth: 계정 조회 또는 신규 생성
        //   3. JWT Access Token + Refresh Token 발급
        //
        // 주의: Apple은 최초 로그인 이후 이메일을 제공하지 않을 수 있다.
        //       이메일이 필요하다면 최초 가입 시점에 반드시 저장해야 한다.
        // ─────────────────────────────────────────────────────────────────
        group.MapPost("/apple/login", async (
            OAuthRequest req, AppleAuthService appleAuth, AccountService svc) =>
        {
            if (string.IsNullOrWhiteSpace(req.IdToken))
                return Results.BadRequest(new { error = "id_token_required" });

            // Apple ID Token 서명 검증 (JWKS 캐시 사용)
            var identity = await appleAuth.ValidateAsync(req.IdToken);
            if (identity is null) return Results.Unauthorized();

            var (response, error) = await svc.LoginOAuthAsync(
                "apple", identity.Value.Sub, identity.Value.Email);

            return error is null ? Results.Ok(response) : Results.StatusCode(403);
        });
    }
}

// 파일 범위 내부 전용 요청 모델
file record OAuthRequest(string IdToken);
