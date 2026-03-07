using Microsoft.IdentityModel.JsonWebTokens;
using Microsoft.IdentityModel.Tokens;

namespace Login.Services;

/// <summary>
/// Apple Sign In ID Token 검증 서비스.
///
/// 동작 방식:
///   1. 클라이언트가 Apple Sign In SDK로 발급받은 ID Token(JWT)을 서버로 전달한다.
///   2. Apple의 공개키 목록(JWKS)을 <see cref="JwksUri"/>에서 다운로드한다.
///      JWKS는 최대 12시간 동안 메모리에 캐싱되며, 이중 체크 락으로 동시성을 보호한다.
///   3. <see cref="JsonWebTokenHandler"/>가 JWKS로 서명을 검증하고
///      발급자(iss == "https://appleid.apple.com"), 만료 시각, Audience를 확인한다.
///   4. appsettings.json에 <c>Auth:Apple:AppBundleId</c>가 설정되어 있으면
///      Audience 검증을 활성화한다. (미설정 시 생략 — 개발 환경 편의)
///   5. 검증 성공 시 Apple 고유 사용자 식별자(sub)와 이메일을 반환한다.
///      이메일은 사용자가 "이메일 숨기기"를 선택한 경우 Apple 중계 주소가 올 수 있다.
///
/// 주의:
///   Apple은 JWKS 키를 주기적으로 교체할 수 있으므로 캐시 만료 전에도
///   서명 검증 실패 시 재시도 로직을 추가하는 것이 권장된다.
/// </summary>
public sealed class AppleAuthService(IConfiguration cfg, ILogger<AppleAuthService> logger)
{
    // Apple 공개키(JSON Web Key Set) 엔드포인트
    private const string JwksUri     = "https://appleid.apple.com/auth/keys";
    // Apple ID Token의 발급자(iss) 고정값
    private const string AppleIssuer = "https://appleid.apple.com";

    // appsettings.json: Auth:Apple:AppBundleId (예: "com.example.mygame")
    private readonly string?            _appBundleId = cfg["Auth:Apple:AppBundleId"];
    private readonly JsonWebTokenHandler _handler    = new();
    private readonly HttpClient          _http       = new();
    // JWKS 갱신 시 동시 요청이 몰리는 것을 방지하기 위한 뮤텍스 (최대 동시 접근 1)
    private readonly SemaphoreSlim       _lock       = new(1, 1);

    // 메모리 캐시: Apple JWKS와 만료 시각
    private JsonWebKeySet? _jwks;
    private DateTime       _jwksExpiry = DateTime.MinValue;

    /// <summary>
    /// Apple ID Token을 검증하고 사용자 식별 정보를 반환한다.
    /// </summary>
    /// <param name="idToken">클라이언트가 Apple SDK에서 발급받은 ID Token 문자열</param>
    /// <returns>
    /// 검증 성공: (Sub: Apple 고유 사용자 ID, Email: 이메일 또는 null)<br/>
    /// 검증 실패: null (만료, 서명 불일치, 발급자 불일치, Audience 불일치 등)
    /// </returns>
    public async Task<(string Sub, string? Email)?> ValidateAsync(string idToken)
    {
        try
        {
            var keys = await GetJwksAsync();

            var parameters = new TokenValidationParameters
            {
                ValidateIssuerSigningKey = true,
                IssuerSigningKeys        = keys.Keys,       // Apple 공개키 목록

                ValidateIssuer           = true,
                ValidIssuer              = AppleIssuer,     // "https://appleid.apple.com"

                // AppBundleId 미설정 시 개발 편의를 위해 Audience 검증 비활성화
                ValidateAudience         = !string.IsNullOrEmpty(_appBundleId),
                ValidAudience            = _appBundleId,

                ValidateLifetime         = true,
                // 클라이언트·서버 시계 오차 허용 범위 (5분)
                ClockSkew                = TimeSpan.FromMinutes(5),
            };

            var result = await _handler.ValidateTokenAsync(idToken, parameters);
            if (!result.IsValid) return null;

            // "sub": Apple의 영구 고유 사용자 식별자 (팀 ID 기준으로 고정)
            result.Claims.TryGetValue("sub",   out var sub);
            // "email": 실제 이메일 또는 Apple 중계 주소(예: abc123@privaterelay.appleid.com)
            result.Claims.TryGetValue("email", out var email);
            if (sub is null) return null;

            return (sub.ToString()!, email?.ToString());
        }
        catch (Exception ex)
        {
            logger.LogWarning("Apple token validation failed: {Message}", ex.Message);
            return null;
        }
    }

    /// <summary>
    /// Apple JWKS를 메모리 캐시에서 반환하거나, 만료 시 재다운로드한다.
    /// 12시간 캐시 + 이중 체크 락(double-checked locking)으로 동시성 안전하게 처리.
    /// </summary>
    private async Task<JsonWebKeySet> GetJwksAsync()
    {
        // 1차 확인: 락 없이 캐시 유효성 검사 (성능 최적화)
        if (_jwks is not null && DateTime.UtcNow < _jwksExpiry)
            return _jwks;

        await _lock.WaitAsync();
        try
        {
            // 2차 확인: 락 획득 후 다른 스레드가 이미 갱신했는지 재확인
            if (_jwks is not null && DateTime.UtcNow < _jwksExpiry)
                return _jwks;

            // Apple JWKS 엔드포인트에서 최신 공개키 목록 다운로드
            var json = await _http.GetStringAsync(JwksUri);
            _jwks       = new JsonWebKeySet(json);
            // 다음 갱신 시각: 12시간 후
            _jwksExpiry = DateTime.UtcNow.AddHours(12);
            return _jwks;
        }
        finally
        {
            _lock.Release();
        }
    }
}
