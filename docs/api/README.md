# API Documentation

Makga 서버들이 제공하는 모든 REST API 문서입니다.

---

## 📚 API 가이드

### [Login Server](./LOGIN_API.md) - 포트 5000
인증 및 토큰 관리 서비스

**주요 기능:**
- 이메일/패스워드 회원가입 및 로그인
- Google/Apple OAuth 로그인
- 게스트 로그인
- Access Token / Refresh Token 관리
- 토큰 갱신 및 폐기

**Endpoints:**
- `POST /v1/auth/self/register` — 회원가입
- `POST /v1/auth/self/login` — 로그인
- `POST /v1/auth/guest/login` — 게스트 로그인
- `POST /v1/auth/google/login` — Google OAuth
- `POST /v1/auth/apple/login` — Apple OAuth
- `POST /v1/auth/token/refresh` — 토큰 갱신
- `POST /v1/auth/token/revoke` — 토큰 폐기

**응답 모델:** LoginResponse (accountUid, accessToken, refreshToken, sessionToken, loginType)

[📖 상세 문서](./LOGIN_API.md)

---

### [Ranking Server](./RANKING_API.md) - 포트 5001
Redis 기반 리더보드 시스템

**주요 기능:**
- 다중 리더보드 지원 (일간, 주간, 월간, 전체)
- 점수 업데이트
- 상위 순위 조회 (캐싱)
- 개별 사용자 순위 조회

**Endpoints:**
- `POST /{leaderboard}/score` — 점수 업데이트
- `GET /{leaderboard}/top?count=10` — 상위 순위 조회
- `GET /{leaderboard}/user/{userId}` — 사용자 순위 조회

**응답 모델:** RankingEntry (rank, userId, score, timestamp)

[📖 상세 문서](./RANKING_API.md)

---

### [World Server](./WORLD_API.md) - 포트 5002
게임 서버 관리 및 플레이어 세션 추적 (Internal API)

**주요 기능:**
- 게임 서버 등록 및 하트비트
- 플레이어 입장/퇴장 추적
- 사용 가능한 게임 서버 발견
- 서버 상태 모니터링

**Endpoints:**
- `POST /server/register` — 서버 등록
- `PUT /server/heartbeat` — 하트비트
- `POST /player/enter` — 플레이어 입장
- `POST /player/leave` — 플레이어 퇴장
- `POST /find-server` — 서버 발견

**응답 모델:** GameServerInfo (serverId, worldId, host, port, status, ...)

[📖 상세 문서](./WORLD_API.md)

---

## 🔗 Server Architecture

```
Client
  ├─ POST /v1/auth/self/login (Login Server:5000)
  ├─ Response: { accessToken, refreshToken, sessionToken, accountUid }
  │
  ├─ POST /v1/internal/find-server (World Server:5002)
  ├─ Response: { host, port, serverId }
  │
  └─ Connect to GameServer (TCP)
     └─ Send sessionToken for authentication

Ranking Integration:
  └─ POST /v1/ranking/{leaderboard}/score (Ranking Server:5001)
```

---

## 🛠️ Common Response Format

### Success Response (2xx)
```json
{
  "data": { /* 응답 데이터 */ },
  "message": "operation successful"
}
```

### Error Response (4xx, 5xx)
```json
{
  "error": "error_code",
  "message": "Human-readable error description"
}
```

---

## 🔐 Authentication

### Access Token (JWT)
- **Header:** `Authorization: Bearer {accessToken}`
- **TTL:** 기본 15분
- **Usage:** HTTP API 인증용

### Refresh Token
- **TTL:** 기본 30일
- **Storage:** 클라이언트 안전 저장소
- **Usage:** Access Token 갱신용

### Session Token
- **Format:** 32자 hex string
- **Usage:** 게임 서버 TCP 연결 인증용
- **Storage:** 계정 DB

---

## 📊 Rate Limiting

- **Login Server:** 개별 엔드포인트별로 제한 (상세: [LOGIN_API.md](./LOGIN_API.md))
- **Ranking Server:** 점수 업데이트 10req/sec, 조회 제한 없음
- **World Server:** 내부 API, 제한 없음

---

## 🚀 Quick Start

### 1. 사용자 인증
```bash
curl -X POST http://localhost:5000/v1/auth/self/login \
  -H "Content-Type: application/json" \
  -d '{"email":"user@example.com","password":"password"}'
```

응답:
```json
{
  "accountUid": 12345,
  "accessToken": "eyJ...",
  "refreshToken": "abc...",
  "sessionToken": "xyz...",
  "loginType": "self"
}
```

### 2. 게임 서버 찾기
```bash
curl -X POST http://localhost:5002/v1/internal/find-server \
  -H "Content-Type: application/json" \
  -d '{"worldId":"world-1"}'
```

응답:
```json
{
  "host": "192.168.1.100",
  "port": 9001,
  "serverId": "game-server-01"
}
```

### 3. 점수 업데이트
```bash
curl -X POST http://localhost:5001/v1/ranking/weekly/score \
  -H "Content-Type: application/json" \
  -d '{"userId":"player123","score":1500}'
```

### 4. 랭킹 조회
```bash
curl http://localhost:5001/v1/ranking/weekly/top?count=10
```

---

## 📝 API 명명 규칙

- **Base Path:** `/v1/` (API 버전)
- **Internal APIs:** `/v1/internal/` (서버 간 통신용)
- **Resource Names:** snake_case 또는 kebab-case
- **HTTP Methods:**
  - `POST` — 생성/상태 변경
  - `GET` — 조회
  - `PUT` — 업데이트
  - `DELETE` — 삭제

---

## 🐛 Error Codes

| Code | Description |
|------|-------------|
| `invalid_credentials` | 인증 정보 오류 |
| `email_already_exists` | 중복된 이메일 |
| `device_id_required` | deviceId 필수 |
| `id_token_required` | idToken 필수 |
| `refresh_token_required` | refreshToken 필수 |
| `server_not_found` | 서버 미등록 |
| `no_available_server` | 사용 가능한 서버 없음 |

---

## 🔄 Token Refresh Flow

```
1. Client sends accessToken in Authorization header
   Authorization: Bearer {accessToken}

2. Server validates token
   ├─ Valid: Process request
   └─ Expired: Return 401 Unauthorized

3. Client receives 401
   ├─ Call POST /v1/auth/token/refresh
   ├─ Send: { "refreshToken": "..." }
   └─ Receive new accessToken + refreshToken

4. Retry original request with new accessToken
```

---

## 📞 Support & Issues

각 API 문서의 상세 정보:
- [Login Server API](./LOGIN_API.md)
- [Ranking Server API](./RANKING_API.md)
- [World Server API](./WORLD_API.md)

API 변경 이력은 커밋 메시지를 참고하세요.

---

## 🎯 Version History

- **v1.0** (2024-03-07) — Initial API documentation
  - Login Server: 5 endpoints
  - Ranking Server: 3 endpoints
  - World Server: 5 endpoints
