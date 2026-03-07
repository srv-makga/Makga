using Login.Services;

namespace Login.Endpoints;

// Auth:EnableTestLogin = true 일 때만 EndpointMapper 에서 등록됨.
// 운영 환경(production)에서는 반드시 false 로 설정할 것.
/// <summary>
/// 테스트 전용 로그인 엔드포인트.
/// QA·개발 환경에서 패스워드 없이 미리 등록된 testKey로 빠르게 로그인할 때 사용한다.
/// </summary>
public static class TestEndpoints
{
    public static void Map(RouteGroupBuilder group)
    {
        // POST /v1/auth/test/login
        // Body: { "testKey": "tester1" }
        // 성공: 200 OK + LoginResponse
        // 실패: 400 testKey 누락, 500 DB 오류 (testKey 미등록 포함)
        group.MapPost("/test/login", async (TestRequest req, AccountService svc) =>
        {
            if (string.IsNullOrWhiteSpace(req.TestKey))
                return Results.BadRequest(new { error = "test_key_required" });

            var (response, error) = await svc.LoginTestAsync(req.TestKey);
            return error is null ? Results.Ok(response) : Results.StatusCode(500);
        });
    }
}

file record TestRequest(string TestKey);
