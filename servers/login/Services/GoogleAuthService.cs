using Google.Apis.Auth;

namespace Login.Services;

/// <summary>
/// Google Sign-In ID Token 검증 서비스.
///
/// 동작 방식:
///   1. 클라이언트가 Google Sign-In SDK로 발급받은 ID Token(JWT)을 서버로 전달한다.
///   2. <see cref="GoogleJsonWebSignature.ValidateAsync"/> 가 Google 공개키(자동 갱신)로
///      서명을 검증하고 토큰 만료 여부·발급자(iss)를 확인한다.
///   3. appsettings.json에 <c>Auth:Google:ClientId</c>가 설정되어 있으면
///      Audience(aud) 클레임이 ClientId와 일치하는지 추가로 검증한다.
///      (미설정 시 Audience 검증 생략 — 개발 환경 편의를 위한 옵션)
///   4. 검증 성공 시 Google 고유 사용자 식별자(sub)와 이메일을 반환한다.
///      sub는 영구 불변 식별자이므로 DB의 oauth_uid 컬럼에 저장한다.
///
/// 의존성: Google.Apis.Auth NuGet 패키지 (Google 공개키 캐싱 내장)
/// </summary>
public sealed class GoogleAuthService(IConfiguration cfg, ILogger<GoogleAuthService> logger)
{
    // appsettings.json: Auth:Google:ClientId
    // null이면 Audience 검증을 건너뛴다 (개발 환경에서 ClientId 없이 테스트 가능).
    private readonly string? _clientId = cfg["Auth:Google:ClientId"];

    /// <summary>
    /// Google ID Token을 검증하고 사용자 식별 정보를 반환한다.
    /// </summary>
    /// <param name="idToken">클라이언트가 Google SDK에서 발급받은 ID Token 문자열</param>
    /// <returns>
    /// 검증 성공: (Sub: Google 고유 사용자 ID, Email: 이메일 주소 또는 null)<br/>
    /// 검증 실패: null (만료, 서명 불일치, Audience 불일치 등)
    /// </returns>
    public async Task<(string Sub, string? Email)?> ValidateAsync(string idToken)
    {
        try
        {
            // ClientId가 설정된 경우에만 Audience 검증 활성화
            var settings = string.IsNullOrEmpty(_clientId)
                ? new GoogleJsonWebSignature.ValidationSettings()
                : new GoogleJsonWebSignature.ValidationSettings { Audience = [_clientId] };

            // 라이브러리가 내부적으로 Google의 공개키 목록(JWKS)을 캐싱하여 검증한다.
            var payload = await GoogleJsonWebSignature.ValidateAsync(idToken, settings);

            // sub: Google의 영구 고유 사용자 식별자 (변경되지 않음)
            return (payload.Subject, payload.Email);
        }
        catch (InvalidJwtException ex)
        {
            // 토큰 만료, 서명 오류, 형식 불량 등 모든 JWT 검증 실패를 여기서 처리
            logger.LogWarning("Google token validation failed: {Message}", ex.Message);
            return null;
        }
    }
}
