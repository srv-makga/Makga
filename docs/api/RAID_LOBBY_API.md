# Raid Lobby Server API

**Base URL:** `http://localhost:5017`  
**Port:** 5017  
**인증 방식:** JWT Bearer Token

---

## 개요

레이드 로비 서버는 레이드 파티 생성, 참가, 매칭, 레이드 시작을 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 레이드 로비 엔드포인트 (JWT 인증 필수)

### GET `/v1/raid/lobbies` — 공개 레이드 로비 목록

**Headers:** `Authorization: Bearer {token}`

**Query Parameters**

| 파라미터 | 타입 | 설명 |
|---|---|---|
| `raid_id` | int? | 특정 레이드 ID 필터 |
| `difficulty` | string? | `normal` / `hard` / `extreme` |

**Response 200 OK**
```json
{
  "lobbies": [
    {
      "lobby_id": "lobby_001",
      "raid_id": 10,
      "raid_name": "용의 둥지",
      "difficulty": "hard",
      "leader_name": "홍길동",
      "current_members": 3,
      "max_members": 8,
      "min_level": 80,
      "created_at": "2026-03-29T12:00:00Z"
    }
  ]
}
```

---

### POST `/v1/raid/lobbies` — 레이드 로비 생성

**Request Body**
```json
{
  "raid_id": 10,
  "difficulty": "hard",
  "max_members": 8,
  "min_level": 80,
  "is_public": true,
  "description": "빠른 클리어 원하시는 분"
}
```

**Response 200 OK**
```json
{ "lobby_id": "lobby_002" }
```

---

### POST `/v1/raid/lobbies/{id}/join` — 레이드 로비 참가

**Path Parameters:** `id` (string) — 로비 ID

**Response 200 OK**
```json
{ "success": true, "current_members": 4 }
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 400 | 레벨 미달 / 정원 초과 |
| 404 | 로비를 찾을 수 없음 |

---

### DELETE `/v1/raid/lobbies/{id}/leave` — 레이드 로비 이탈

**Path Parameters:** `id` (string)

**Response 200 OK**

---

### POST `/v1/raid/lobbies/{id}/start` — 레이드 시작 (리더 전용)

**Path Parameters:** `id` (string)

**Response 200 OK**
```json
{
  "success": true,
  "game_server_host": "game-server-01.makga.internal",
  "game_server_port": 9001,
  "instance_id": "raid_instance_001"
}
```

---

### POST `/v1/raid/match` — 자동 매칭

**Request Body**
```json
{ "raid_id": 10, "difficulty": "normal" }
```

**Response 200 OK**
```json
{ "queue_token": "match_q_001", "estimated_wait_seconds": 60 }
```

---

## 관련 문서

- [아키텍처 구조도](../architecture/RAID_LOBBY_ARCHITECTURE.md)
- [World API](WORLD_API.md)
