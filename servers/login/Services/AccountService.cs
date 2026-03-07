using Login.Models;
using Login.Repository;

namespace Login.Services;

/// <summary>
/// 계정 로그인·등록의 비즈니스 로직을 담당하는 서비스.
/// 모든 로그인 방식에서 성공 시 Access Token + Refresh Token + Session Token을 발급하고
/// last_login 시각을 갱신한다.
/// </summary>
public sealed class AccountService(
    AccountRepository   repo,
    JwtService          jwt,
    RefreshTokenService refreshTokens,
    IConfiguration      cfg)
{
    // Auth:SessionTokenDays: 내부 서버 세션 토큰 유효 기간 (기본 30일)
    private readonly int _sessionTokenDays =
        int.TryParse(cfg["Auth:SessionTokenDays"], out var d) ? d : 30;

    private static bool IsActive(string status) => status == "active";

    /// <summary>
    /// Access Token(JWT), Refresh Token(Redis), Session Token(MySQL)을 발급하고
    /// last_login을 갱신한다. 모든 로그인 경로의 공통 마무리 단계.
    ///
    /// 토큰 용도:
    ///   AccessToken  — HTTP API 인증 (Authorization 헤더, 단기)
    ///   RefreshToken — Access Token 갱신 (장기, Redis 저장)
    ///   SessionToken — 게임 서버 등 내부 서버 TCP 연결 인증 (account_db 저장)
    /// </summary>
    private async Task<LoginResponse> IssueTokensAsync(long accountUid, string loginType)
    {
        var accessToken  = jwt.IssueAccessToken(accountUid, loginType);
        var refreshToken = await refreshTokens.IssueAsync(accountUid, loginType);

        // 내부 서버 연결용 세션 토큰 생성 및 DB 저장
        // Guid.NewGuid().ToString("N"): 32자 hex 문자열 (예: "d3b07384d113edec49eaa6238ad5ff00")
        var sessionToken = Guid.NewGuid().ToString("N");
        var expiresAt    = DateTime.UtcNow.AddDays(_sessionTokenDays);
        await repo.UpsertSessionTokenAsync(accountUid, sessionToken, loginType, expiresAt);

        await repo.UpdateLastLoginAsync(accountUid);
        return new LoginResponse(accountUid, accessToken, refreshToken, sessionToken, loginType);
    }

    /// <summary>
    /// 이메일·패스워드 방식으로 새 계정을 생성한다.
    /// 패스워드는 BCrypt(work factor 12)로 해싱하여 DB에 저장한다.
    /// 동일 이메일이 이미 존재하면 "email_already_exists" 오류를 반환한다.
    /// </summary>
    public async Task<(LoginResponse? Response, string? Error)> RegisterSelfAsync(
        string email, string password)
    {
        // BCrypt work factor 12: 약 300ms 소요 (브루트포스 공격 억제 목적)
        var hash = BCrypt.Net.BCrypt.HashPassword(password, workFactor: 12);
        var (uid, errorCode) = await repo.RegisterSelfAsync(email, hash);

        // DB SP의 p_error_code: -1 = 이메일 중복
        if (errorCode == -1) return (null, "email_already_exists");

        var response = await IssueTokensAsync(uid, "self");
        return (response, null);
    }

    /// <summary>
    /// 이메일·패스워드 방식으로 로그인한다.
    /// BCrypt.Verify로 해시를 비교하며, 계정 상태(banned 등)를 확인한다.
    /// 이메일 미존재와 패스워드 불일치 모두 "invalid_credentials"를 반환해
    /// 사용자 열거(User Enumeration) 공격을 방지한다.
    /// </summary>
    public async Task<(LoginResponse? Response, string? Error)> LoginSelfAsync(
        string email, string password)
    {
        var row = await repo.GetSelfLoginAsync(email);
        if (row is null) return (null, "invalid_credentials");

        // 계정 정지·휴면 등 비활성 상태 처리
        if (!IsActive(row.Status)) return (null, $"account_{row.Status}");

        if (!BCrypt.Net.BCrypt.Verify(password, row.PasswordHash))
            return (null, "invalid_credentials");

        var response = await IssueTokensAsync(row.AccountUid, "self");
        return (response, null);
    }

    /// <summary>
    /// Google / Apple OAuth 로그인 처리.
    /// oauthUid(플랫폼 고유 식별자)로 계정을 조회하거나, 없으면 신규 생성(Upsert)한다.
    /// </summary>
    /// <param name="loginType">플랫폼 식별자 ("google" | "apple")</param>
    /// <param name="oauthUid">플랫폼에서 발급한 고유 사용자 ID (Google: sub, Apple: sub)</param>
    /// <param name="email">플랫폼에서 제공한 이메일 (nullable)</param>
    public async Task<(LoginResponse? Response, string? Error)> LoginOAuthAsync(
        string loginType, string oauthUid, string? email)
    {
        // DB에서 (loginType, oauthUid) 쌍으로 계정 조회 또는 신규 생성
        var row = await repo.UpsertOAuthAsync(loginType, oauthUid, email);
        if (row is null) return (null, "db_error");
        if (!IsActive(row.Status)) return (null, $"account_{row.Status}");

        var response = await IssueTokensAsync(row.AccountUid, loginType);
        return (response, null);
    }

    /// <summary>
    /// 게스트 로그인 처리.
    /// deviceId(클라이언트 UUID)로 계정을 조회하거나 없으면 신규 생성(Upsert)한다.
    /// 게스트 계정은 OAuth 연동 전까지 deviceId에 귀속된다.
    /// </summary>
    public async Task<(LoginResponse? Response, string? Error)> LoginGuestAsync(string deviceId)
    {
        var row = await repo.LoginGuestAsync(deviceId);
        if (row is null) return (null, "db_error");
        if (!IsActive(row.Status)) return (null, $"account_{row.Status}");

        var response = await IssueTokensAsync(row.AccountUid, "guest");
        return (response, null);
    }

    /// <summary>
    /// 테스트 전용 로그인 (Auth:EnableTestLogin = true 일 때만 활성화).
    /// testKey는 미리 DB에 등록된 테스트 계정 식별자다.
    /// 운영 환경에서는 반드시 비활성화해야 한다.
    /// </summary>
    public async Task<(LoginResponse? Response, string? Error)> LoginTestAsync(string testKey)
    {
        var row = await repo.LoginTestAsync(testKey);
        if (row is null) return (null, "db_error");
        if (!IsActive(row.Status)) return (null, $"account_{row.Status}");

        var response = await IssueTokensAsync(row.AccountUid, "test");
        return (response, null);
    }
}

