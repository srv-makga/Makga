using Login.Services;

namespace Login.Endpoints;

/// <summary>
/// 이메일·패스워드 자체 로그인 엔드포인트.
/// BCrypt(work factor 12)로 패스워드를 해싱·검증한다.
/// </summary>
public static class SelfEndpoints
{
    public static void Map(RouteGroupBuilder group)
    {
        // POST /v1/auth/self/register
        // Body: { "email": "...", "password": "..." }
        // 성공: 200 OK + LoginResponse (Access Token + Refresh Token)
        // 실패: 400 email/password 누락, 409 이메일 중복
        group.MapPost("/self/register", async (SelfRequest req, AccountService svc) =>
        {
            if (string.IsNullOrWhiteSpace(req.Email) || string.IsNullOrWhiteSpace(req.Password))
                return Results.BadRequest(new { error = "email_and_password_required" });

            var (response, error) = await svc.RegisterSelfAsync(req.Email, req.Password);
            return error is null
                ? Results.Ok(response)
                : Results.Conflict(new { error });   // error == "email_already_exists"
        });

        // POST /v1/auth/self/login
        // Body: { "email": "...", "password": "..." }
        // 성공: 200 OK + LoginResponse (Access Token + Refresh Token)
        // 실패: 400 email/password 누락, 401 이메일 미존재 또는 패스워드 불일치
        // 보안: 이메일 미존재와 패스워드 불일치 모두 401 + "invalid_credentials" 로 통일
        //       (User Enumeration 공격 방지)
        group.MapPost("/self/login", async (SelfRequest req, AccountService svc) =>
        {
            if (string.IsNullOrWhiteSpace(req.Email) || string.IsNullOrWhiteSpace(req.Password))
                return Results.BadRequest(new { error = "email_and_password_required" });

            var (response, error) = await svc.LoginSelfAsync(req.Email, req.Password);
            return error is null
                ? Results.Ok(response)
                : Results.Unauthorized();
        });
    }
}

file record SelfRequest(string Email, string Password);
