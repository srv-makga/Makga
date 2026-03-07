using System.Text;
using Microsoft.IdentityModel.JsonWebTokens;
using Microsoft.IdentityModel.Tokens;

namespace Login.Services;

/// <summary>
/// JWT Access Token 발급 및 검증 서비스.
///
/// 토큰 구조:
///   Header: { "alg": "HS256", "typ": "JWT" }
///   Payload: { "uid": {accountUid}, "lt": {loginType}, "nbf": ..., "iat": ..., "exp": ... }
///   Signature: HMAC-SHA256(secret)
///
/// 설정 (appsettings.json):
///   Jwt:Secret              — HMAC-SHA256 서명 키 (32자 이상 권장)
///   Jwt:AccessTokenMinutes  — Access Token 유효 시간 (기본 15분)
///
/// Claims:
///   uid — 내부 계정 고유 ID (long)
///   lt  — 로그인 방식 (self | google | apple | guest | test)
/// </summary>
public sealed class JwtService
{
    private readonly JsonWebTokenHandler _handler = new();
    private readonly SigningCredentials  _credentials;
    private readonly int _accessTokenMinutes;

    public JwtService(IConfiguration cfg)
    {
        var secret = cfg["Jwt:Secret"]
            ?? throw new InvalidOperationException("Jwt:Secret is required");

        // HMAC-SHA256 서명 키 생성 (UTF-8 바이트 배열 → SymmetricSecurityKey)
        var key = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(secret));
        _credentials       = new SigningCredentials(key, SecurityAlgorithms.HmacSha256);
        _accessTokenMinutes = int.TryParse(cfg["Jwt:AccessTokenMinutes"], out var m) ? m : 15;
    }

    /// <summary>
    /// 계정 UID와 로그인 방식을 포함하는 Access Token을 발급한다.
    /// </summary>
    /// <param name="accountUid">내부 계정 고유 ID</param>
    /// <param name="loginType">로그인 방식 (self | google | apple | guest | test)</param>
    /// <returns>서명된 JWT 문자열</returns>
    public string IssueAccessToken(long accountUid, string loginType)
    {
        var now = DateTime.UtcNow;
        return _handler.CreateToken(new SecurityTokenDescriptor
        {
            Claims = new Dictionary<string, object>
            {
                ["uid"] = accountUid,   // 계정 고유 ID
                ["lt"]  = loginType,    // 로그인 방식 (게임 서버에서 권한 분기에 활용)
            },
            NotBefore          = now,
            IssuedAt           = now,
            Expires            = now.AddMinutes(_accessTokenMinutes),
            SigningCredentials  = _credentials,
        });
    }

    /// <summary>
    /// Access Token을 검증하고 포함된 클레임을 반환한다.
    /// Issuer/Audience 검증은 생략한다 (내부 서비스 간 신뢰 환경).
    /// ClockSkew = 0 으로 설정해 만료 시각을 엄격하게 처리한다.
    /// </summary>
    /// <param name="token">검증할 JWT Access Token 문자열</param>
    /// <returns>
    /// 검증 성공: (AccountUid, LoginType)<br/>
    /// 검증 실패: null (만료, 서명 불일치, 클레임 누락 등)
    /// </returns>
    public async Task<(long AccountUid, string LoginType)?> ValidateAccessTokenAsync(string token)
    {
        var result = await _handler.ValidateTokenAsync(token, new TokenValidationParameters
        {
            ValidateIssuerSigningKey = true,
            IssuerSigningKey         = _credentials.Key,
            ValidateIssuer           = false,   // 단일 서비스 내부 토큰이므로 Issuer 불필요
            ValidateAudience         = false,   // Audience 미설정
            ClockSkew                = TimeSpan.Zero, // 만료 허용 오차 없음
        });

        if (!result.IsValid) return null;

        result.Claims.TryGetValue("uid", out var uid);
        result.Claims.TryGetValue("lt",  out var lt);
        if (uid is null || lt is null) return null;

        return (Convert.ToInt64(uid), lt.ToString()!);
    }
}
