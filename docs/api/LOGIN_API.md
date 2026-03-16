# Login Server API Documentation

**Base URL:** `http://localhost:5000/v1/auth`  
**Port:** 5000

---

## Overview

Login Server는 인증 및 토큰 관리 기능을 제공합니다. 지원하는 로그인 방식:
- **Self (Email/Password)** — 자체 계정 로그인
- **Guest** — 기기 ID 기반 게스트 로그인
- **Google OAuth** — Google ID Token 기반
- **Apple OAuth** — Apple ID Token 기반
- **Token Management** — Refresh/Revoke

---

## 1. Self (Email/Password) Authentication

### 1.1 회원가입
**POST** `/self/register`

**Description:** 이메일과 패스워드로 새 계정을 생성합니다. 패스워드는 BCrypt(work factor 12)로 해싱됩니다.

**Request:**
```json
{
  "email": "user@example.com",
  "password": "secure_password"
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| email | string | ✓ | 사용자 이메일 (중복 불가) |
| password | string | ✓ | 사용자 패스워드 (최소 권장: 8자 이상) |

**Response (200 OK):**
```json
{
  "accountUid": 12345,
  "accessToken": "eyJhbGciOiJIUzI1NiIs...",
  "refreshToken": "abc123def456...",
  "sessionToken": "f1a2b3c4d5e6f7g8h9i0...",
  "loginType": "self"
}
```

**Error Responses:**
- `400 Bad Request` — email/password 누락
- `409 Conflict` — 이메일이 이미 존재 (`error: "email_already_exists"`)

---

### 1.2 로그인
**POST** `/self/login`

**Description:** 이메일과 패스워드로 기존 계정에 로그인합니다.

**Request:**
```json
{
  "email": "user@example.com",
  "password": "secure_password"
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| email | string | ✓ | 등록된 사용자 이메일 |
| password | string | ✓ | 사용자 패스워드 |

**Response (200 OK):**
```json
{
  "accountUid": 12345,
  "accessToken": "eyJhbGciOiJIUzI1NiIs...",
  "refreshToken": "abc123def456...",
  "sessionToken": "f1a2b3c4d5e6f7g8h9i0...",
  "loginType": "self"
}
```

**Error Responses:**
- `400 Bad Request` — email/password 누락
- `401 Unauthorized` — 이메일 미존재 또는 패스워드 불일치 (User Enumeration 공격 방지 목적으로 두 경우 모두 `401` 반환)

---

## 2. Guest Login

### 2.1 게스트 로그인
**POST** `/guest/login`

**Description:** 기기 ID(클라이언트 UUID)로 게스트 계정을 조회하거나 새로 생성합니다.

**주의:** 게스트 계정은 OAuth 연동 전까지 deviceId에 귀속됩니다. 클라이언트는 deviceId를 영구 저장소에 보관해야 합니다.

**Request:**
```json
{
  "deviceId": "550e8400-e29b-41d4-a716-446655440000"
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| deviceId | string | ✓ | 클라이언트 UUID (고유성 보장) |

**Response (200 OK):**
```json
{
  "accountUid": 12346,
  "accessToken": "eyJhbGciOiJIUzI1NiIs...",
  "refreshToken": "xyz789uvw012...",
  "sessionToken": "a1b2c3d4e5f6g7h8i9j0...",
  "loginType": "guest"
}
```

**Error Responses:**
- `400 Bad Request` — deviceId 누락
- `500 Internal Server Error` — DB 오류

---

## 3. OAuth Authentication (Google)

### 3.1 Google 로그인
**POST** `/google/login`

**Description:** Google ID Token을 검증하고 계정을 조회 또는 생성합니다.

**Client Flow:**
1. 클라이언트가 Google SDK로 로그인 → ID Token 발급
2. ID Token을 이 엔드포인트로 전달
3. 서버가 Google 공개키로 서명 검증 → 계정 조회/생성 → JWT 발급

**Request:**
```json
{
  "idToken": "eyJhbGciOiJSUzI1NiIsImtpZCI6IjEifQ.eyJpc3MiOiJodHRwczovL2FjY291bnRzLmdvb2dsZS5jb20iLCJzdWIiOiIxMTExMTExMTExMTExMTExMTExIiwibmFtZSI6IkpvaG4gRG9lIiwiZW1haWwiOiJqb2huQGV4YW1wbGUuY29tIn0.signature"
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| idToken | string | ✓ | Google ID Token (클라이언트에서 발급) |

**Server Processing:**
1. `GoogleAuthService.ValidateAsync()` — Google 공개키로 서명 검증
   - `payload.Subject` (Google 고유 사용자 ID, 영구 불변)
   - `payload.Email` (Google 이메일)
2. `AccountService.LoginOAuthAsync("google", sub, email)`
   - DB 저장 프로시저 `sp_upsert_oauth` 호출: 계정 조회 또는 신규 생성
3. JWT Access Token + Refresh Token 발급

**Response (200 OK):**
```json
{
  "accountUid": 12347,
  "accessToken": "eyJhbGciOiJIUzI1NiIs...",
  "refreshToken": "ghi345jkl678...",
  "sessionToken": "k1l2m3n4o5p6q7r8s9t0...",
  "loginType": "google"
}
```

**Error Responses:**
- `400 Bad Request` — idToken 필드 누락
- `401 Unauthorized` — Google 토큰 검증 실패 (만료, 서명 오류 등)
- `403 Forbidden` — 계정 정지·비활성 상태

---

## 4. OAuth Authentication (Apple)

### 4.1 Apple 로그인
**POST** `/apple/login`

**Description:** Apple ID Token을 검증하고 계정을 조회 또는 생성합니다.

**Client Flow:**
1. 클라이언트가 Apple SDK로 로그인 → ID Token 발급
2. ID Token을 이 엔드포인트로 전달
3. 서버가 Apple JWKS로 서명 검증 → 계정 조회/생성 → JWT 발급

**Request:**
```json
{
  "idToken": "eyJhbGciOiJSUzI1NiIsImtpZCI6IkFNNzAwOUI1RjZGNkY0MjlCNEJGMjA1QkNBRDQzQzEwQjIzMDNCRjI4In0.eyJpc3MiOiJodHRwczovL2FwcGxlaWQuYXBwbGUuY29tIiwic3ViIjoiMDAwOTMwLmU4ODBkMDgyYTFjMDQ3NTljYjU4YjQ4ZjBkNjQ1ZjAxLjA5MzAiLCJhdWQiOiJjb20uZXhhbXBsZS5hcHAiLCJjIjoiMjAwMDAxLWJ6ZHQzIiwiZW1haWwiOiJ0ZXN0QHByaXZhdGVyZWxheS5hcHBsZWlkLmNvbSIsImVtYWlsX3ZlcmlmaWVkIjoiMSIsImlzdF9wcml2YXRlX2VtYWlsIjoiMSIsImF1dGhfdGltZSI6MTY0NDkwODkzMSwibm9uY2Vfc3VwcG9ydGVkIjp0cnVlfQ.signature"
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| idToken | string | ✓ | Apple ID Token (클라이언트에서 발급) |

**Server Processing:**
1. `AppleAuthService.ValidateAsync()` — Apple JWKS로 서명 검증 (캐시 사용)
   - `claims["sub"]` (Apple 고유 사용자 ID, 팀 기준 영구 불변)
   - `claims["email"]` (실제 이메일 또는 Apple 중계 주소)
2. `AccountService.LoginOAuthAsync("apple", sub, email)`
   - DB 저장 프로시저 `sp_upsert_oauth` 호출: 계정 조회 또는 신규 생성
3. JWT Access Token + Refresh Token 발급

**Response (200 OK):**
```json
{
  "accountUid": 12348,
  "accessToken": "eyJhbGciOiJIUzI1NiIs...",
  "refreshToken": "mno567pqr890...",
  "sessionToken": "u1v2w3x4y5z6a7b8c9d0...",
  "loginType": "apple"
}
```

**Error Responses:**
- `400 Bad Request` — idToken 필드 누락
- `401 Unauthorized` — Apple 토큰 검증 실패 (만료, 서명 오류 등)
- `403 Forbidden` — 계정 정지·비활성 상태

**주의:** Apple은 최초 로그인 이후 이메일을 제공하지 않을 수 있습니다. 이메일이 필요하면 최초 가입 시점에 반드시 저장해야 합니다.

---

## 5. Token Management

### 5.1 Refresh Token 갱신
**POST** `/token/refresh`

**Description:** Refresh Token으로 새로운 Access Token과 Refresh Token을 발급합니다 (Token Rotation).

**Token Rotation 처리:**
1. 기존 Refresh Token을 Redis에서 조회·검증
2. 기존 토큰 즉시 삭제 (재사용 방지)
3. 새 Refresh Token 발급 + Redis 저장
4. 새 Access Token(JWT) 발급

**Request:**
```json
{
  "refreshToken": "abc123def456..."
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| refreshToken | string | ✓ | 클라이언트에서 저장한 Refresh Token |

**Response (200 OK):**
```json
{
  "access_token": "eyJhbGciOiJIUzI1NiIs...",
  "refresh_token": "new_refresh_token_xyz789..."
}
```

**Error Responses:**
- `400 Bad Request` — refreshToken 필드 누락
- `401 Unauthorized` — Refresh Token이 없음·만료됨·검증 실패

**Note:** Token Rotation은 원자적이지 않으므로 극히 드물게 중복 발급이 가능할 수 있습니다.

---

### 5.2 Refresh Token 폐기
**POST** `/token/revoke`

**Description:** Refresh Token을 즉시 무효화합니다. 로그아웃 또는 보안 사고 시 사용됩니다.

**Request:**
```json
{
  "refreshToken": "abc123def456..."
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| refreshToken | string | ✓ | 무효화할 Refresh Token |

**Response (200 OK):**
```json
{}
```

**Error Responses:**
- `400 Bad Request` — refreshToken 필드 누락

**Note:** Redis에서 Refresh Token을 즉시 삭제합니다. 토큰이 이미 없어도 200 OK를 반환하므로 멱등성(Idempotency)이 보장됩니다.

---

## Response Models

### LoginResponse
모든 로그인 성공 응답에서 사용하는 공통 DTO:

```json
{
  "accountUid": 12345,
  "accessToken": "JWT token (단기, 기본 15분)",
  "refreshToken": "장기 토큰 (기본 30일, Redis 저장)",
  "sessionToken": "내부 서버 연결용 세션 토큰 (32자 hex, account_db 저장)",
  "loginType": "self | google | apple | guest | test"
}
```

**Token Usage:**
- **AccessToken (JWT)** — HTTP API 인증용 (Authorization 헤더, 단기)
- **RefreshToken** — Access Token 갱신용 (장기, 안전한 저장소에 보관)
- **SessionToken** — 게임 서버 등 내부 서버 TCP 연결 인증용 불투명 토큰

---

## Security Considerations

1. **User Enumeration Prevention** — Self 로그인 실패 시 "이메일 미존재"와 "패스워드 불일치" 모두 `401 Unauthorized`로 반환
2. **Password Hashing** — BCrypt work factor 12 사용
3. **Token Rotation** — Refresh Token 갱신 시 이전 토큰 즉시 삭제
4. **OAuth Validation** — Google/Apple 공개키로 토큰 서명 검증
5. **Account Status Check** — 로그인 후 계정 상태(active/banned/dormant 등) 검증

---

## Usage Example

### cURL 예제

**Self 회원가입:**
```bash
curl -X POST http://localhost:5000/v1/auth/self/register \
  -H "Content-Type: application/json" \
  -d '{"email":"user@example.com","password":"secure_password"}'
```

**Self 로그인:**
```bash
curl -X POST http://localhost:5000/v1/auth/self/login \
  -H "Content-Type: application/json" \
  -d '{"email":"user@example.com","password":"secure_password"}'
```

**Access Token 갱신:**
```bash
curl -X POST http://localhost:5000/v1/auth/token/refresh \
  -H "Content-Type: application/json" \
  -d '{"refreshToken":"abc123def456..."}'
```

**Refresh Token 폐기:**
```bash
curl -X POST http://localhost:5000/v1/auth/token/revoke \
  -H "Content-Type: application/json" \
  -d '{"refreshToken":"abc123def456..."}'
```

---

## Related Documentation

- [World Server API](./WORLD_API.md) — Player/Server 관리
- [Ranking Server API](./RANKING_API.md) — 랭킹 조회
