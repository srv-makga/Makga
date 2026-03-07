using StackExchange.Redis;

namespace Login.Services;

/// <summary>
/// Redis 기반 Refresh Token 관리 서비스.
///
/// 저장 구조:
///   Key:   "refresh:{token}"  (token = UUID v4, 하이픈 없는 32자 hex 문자열)
///   Value: "{accountUid}|{loginType}"
///   TTL:   Jwt:RefreshTokenDays (기본 30일)
///
/// 토큰 로테이션(Rotation) 전략:
///   갱신 요청 시 기존 토큰을 즉시 삭제하고 새 토큰을 발급한다.
///   이미 사용된 Refresh Token 재사용 시 자동으로 401이 반환되어 탈취 감지에 활용할 수 있다.
///
/// 설정 (appsettings.json):
///   Jwt:RefreshTokenDays — Refresh Token TTL (기본 30일)
/// </summary>
public sealed class RefreshTokenService
{
    private readonly IDatabase _db;
    private readonly int       _refreshTokenDays;

    public RefreshTokenService(IConnectionMultiplexer redis, IConfiguration cfg)
    {
        _db               = redis.GetDatabase();
        _refreshTokenDays = int.TryParse(cfg["Jwt:RefreshTokenDays"], out var d) ? d : 30;
    }

    /// <summary>
    /// 새 Refresh Token을 발급하여 Redis에 저장한 뒤 토큰 문자열을 반환한다.
    /// Redis value 형식: "{accountUid}|{loginType}"
    /// </summary>
    public async Task<string> IssueAsync(long accountUid, string loginType)
    {
        // Guid.NewGuid().ToString("N"): 하이픈 없는 32자 hex 문자열 (예: "d3b07384d113edec...")
        var token = Guid.NewGuid().ToString("N");
        await _db.StringSetAsync(
            $"refresh:{token}",
            $"{accountUid}|{loginType}",
            TimeSpan.FromDays(_refreshTokenDays));
        return token;
    }

    /// <summary>
    /// Refresh Token을 검증하고 포함된 계정 정보를 반환한다.
    /// 토큰이 존재하지 않거나 형식이 맞지 않으면 null을 반환한다.
    /// </summary>
    public async Task<(long AccountUid, string LoginType)?> ValidateAsync(string token)
    {
        var val = await _db.StringGetAsync($"refresh:{token}");
        if (val.IsNullOrEmpty) return null;

        // "{accountUid}|{loginType}" 형식을 파싱
        var parts = val.ToString().Split('|', 2);
        if (parts.Length != 2) return null;
        if (!long.TryParse(parts[0], out var uid)) return null;
        return (uid, parts[1]);
    }

    /// <summary>
    /// Refresh Token을 Redis에서 삭제(폐기)한다.
    /// 로그아웃 또는 보안 이상 감지 시 호출한다.
    /// </summary>
    public Task RevokeAsync(string token)
        => _db.KeyDeleteAsync($"refresh:{token}");

    /// <summary>
    /// 기존 Refresh Token을 폐기하고 새 토큰을 발급하는 Rotation 처리.
    /// 한 번 사용된 토큰은 즉시 무효화되어 재사용 공격을 방지한다.
    /// </summary>
    /// <returns>
    /// 성공: (NewToken, AccountUid, LoginType)<br/>
    /// 실패(토큰 없음·만료): null
    /// </returns>
    public async Task<(string NewToken, long AccountUid, string LoginType)?> RotateAsync(
        string oldToken)
    {
        var data = await ValidateAsync(oldToken);
        if (data is null) return null;

        // 기존 토큰 즉시 삭제 후 새 토큰 발급 (atomic하지 않으나 실용적으로 충분)
        await RevokeAsync(oldToken);
        var newToken = await IssueAsync(data.Value.AccountUid, data.Value.LoginType);
        return (newToken, data.Value.AccountUid, data.Value.LoginType);
    }
}
