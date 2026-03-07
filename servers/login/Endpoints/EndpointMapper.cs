using Login.Endpoints;

namespace Login.Endpoints;

/// <summary>
/// 로그인 서버의 모든 HTTP 엔드포인트를 등록하는 진입점.
///
/// 엔드포인트 구조:
///   GET  /ping                — 헬스 체크 (로드밸런서용)
///   GET  /health              — 상세 헬스 체크
///   GET  /version             — 서버 버전 확인
///   POST /v1/auth/self/...    — 이메일·패스워드 자체 로그인 (SelfEndpoints)
///   POST /v1/auth/google/...  — Google OAuth 로그인 (OAuthEndpoints)
///   POST /v1/auth/apple/...   — Apple Sign In 로그인 (OAuthEndpoints)
///   POST /v1/auth/guest/...   — 게스트 로그인 (GuestEndpoints)
///   POST /v1/auth/token/...   — 토큰 갱신·폐기 (TokenEndpoints)
///   POST /v1/auth/test/...    — 테스트 로그인 (TestEndpoints, 조건부 등록)
/// </summary>
public static class EndpointMapper
{
    public static void MapAll(WebApplication app)
    {
        app.MapGet("/ping",    () => "pong");
        app.MapGet("/health",  () => Results.Ok(new { status = "healthy" }));
        app.MapGet("/version", () => "1.0.0");

        // 모든 인증 엔드포인트를 /v1/auth 경로 그룹 아래 등록
        var auth = app.MapGroup("/v1/auth");
        SelfEndpoints.Map(auth);
        OAuthEndpoints.Map(auth);
        GuestEndpoints.Map(auth);
        TokenEndpoints.Map(auth);

        // 테스트 로그인: appsettings.json의 Auth:EnableTestLogin 이 true 일 때만 등록
        // 운영 환경에서는 반드시 false 로 설정할 것
        var testLoginEnabled = app.Configuration.GetValue<bool>("Auth:EnableTestLogin");
        if (testLoginEnabled)
            TestEndpoints.Map(auth);
    }
}
