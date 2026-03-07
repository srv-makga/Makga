namespace Login.Models;

/// <summary>
/// OAuth / 게스트 로그인처럼 계정 상태만 필요할 때 사용하는 최소 조회 결과.
/// </summary>
/// <param name="AccountUid">내부 계정 고유 ID</param>
/// <param name="Status">계정 상태 (active | banned | dormant …)</param>
public record AccountRow(long AccountUid, string Status);

/// <summary>
/// 자체 로그인(Self)용 조회 결과. 패스워드 해시 비교에 사용.
/// </summary>
/// <param name="AccountUid">내부 계정 고유 ID</param>
/// <param name="PasswordHash">bcrypt 해시 (work factor 12)</param>
/// <param name="Status">계정 상태</param>
public record SelfLoginRow(long AccountUid, string PasswordHash, string Status);

/// <summary>
/// 모든 로그인 성공 응답에 공통으로 사용하는 DTO.
///
/// 토큰 용도:
///   AccessToken   — HTTP API 인증용 JWT (Authorization 헤더, 단기)
///   RefreshToken  — Access Token 갱신용 장기 토큰 (Redis 저장, 안전한 저장소에 보관)
///   SessionToken  — 게임 서버 등 내부 서버 TCP 연결 인증용 불투명 토큰 (account_db 저장)
/// </summary>
/// <param name="AccountUid">내부 계정 고유 ID</param>
/// <param name="AccessToken">JWT Access Token (단기, 기본 15분)</param>
/// <param name="RefreshToken">Refresh Token (장기, 기본 30일, Redis 저장)</param>
/// <param name="SessionToken">내부 서버 연결용 세션 토큰 (32자 hex, account_db 저장)</param>
/// <param name="LoginType">로그인 방식 식별자 (self | google | apple | guest | test)</param>
public record LoginResponse(
    long   AccountUid,
    string AccessToken,
    string RefreshToken,
    string SessionToken,
    string LoginType);
