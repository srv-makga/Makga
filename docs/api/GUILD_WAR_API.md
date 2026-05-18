# Guild War Server API

**Base URL:** `http://localhost:5018`  
**Port:** 5018  
**인증 방식:** JWT Bearer Token

---

## 개요

길드전 서버는 길드전 신청, 매칭, 진행 상황 조회, 결과 처리를 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 길드전 엔드포인트 (JWT 인증 필수)

### GET `/v1/guild-war/schedule` — 길드전 일정 조회

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "next_war": {
    "registration_start": "2026-03-30T18:00:00Z",
    "registration_end": "2026-03-30T20:00:00Z",
    "war_start": "2026-03-30T21:00:00Z",
    "war_end": "2026-03-30T23:00:00Z"
  }
}
```

---

### POST `/v1/guild-war/register` — 길드전 참가 신청 (길드장 전용)

**Request Body**
```json
{ "guild_id": 201, "member_uids": [1001, 1002, 1003, 1004, 1005] }
```

**Response 200 OK**
```json
{ "registration_id": 3001, "message": "길드전 참가 신청 완료" }
```

---

### GET `/v1/guild-war/current` — 현재 진행 중인 길드전 조회

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "war_id": 5001,
  "my_guild": { "guild_id": 201, "name": "용사단", "score": 1500 },
  "opponent_guild": { "guild_id": 202, "name": "검은 날개", "score": 1200 },
  "time_remaining_seconds": 3600,
  "status": "in_progress"
}
```

---

### GET `/v1/guild-war/history` — 길드전 이력

**Headers:** `Authorization: Bearer {token}`

**Query Parameters:** `page` (int, 기본값 1)

**Response 200 OK** — 길드전 이력 목록

---

### GET `/v1/guild-war/rankings` — 길드전 랭킹

**Query Parameters:** `season` (int?) — 시즌 ID (없으면 현재 시즌)

**Response 200 OK** — 길드전 랭킹 목록

---

## 관련 문서

- [아키텍처 구조도](../architecture/GUILD_WAR_ARCHITECTURE.md)
- [Ranking API](RANKING_API.md)
