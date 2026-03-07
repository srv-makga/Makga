using Login.Services;

namespace Login.Endpoints;

/// <summary>
/// 게스트 로그인 엔드포인트.
/// deviceId(클라이언트 UUID)로 계정을 조회하거나 없으면 신규 생성한다.
/// 게스트 계정은 OAuth 연동 전까지 deviceId에 귀속되므로
/// 클라이언트는 deviceId를 영구 저장소에 보관해야 한다.
/// </summary>
public static class GuestEndpoints
{
    public static void Map(RouteGroupBuilder group)
    {
        // POST /v1/auth/guest/login
        // Body: { "deviceId": "<client UUID>" }
        // 성공: 200 OK + LoginResponse
        // 실패: 400 deviceId 누락, 500 DB 오류
        group.MapPost("/guest/login", async (GuestRequest req, AccountService svc) =>
        {
            if (string.IsNullOrWhiteSpace(req.DeviceId))
                return Results.BadRequest(new { error = "device_id_required" });

            var (response, error) = await svc.LoginGuestAsync(req.DeviceId);
            return error is null ? Results.Ok(response) : Results.StatusCode(500);
        });
    }
}

file record GuestRequest(string DeviceId);
