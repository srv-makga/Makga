# Replay Server API

**Base URL:** `http://localhost:5025`  
**Port:** 5025  
**인증 방식:** JWT Bearer Token (공개) / 내부 네트워크 전용 (전투 기록 제출)

---

## 개요

리플레이 서버는 전투 기록 제출, 전투 이력 조회, 리플레이 데이터 다운로드, 전투 통계, DPS 리더보드를 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 내부 엔드포인트 (게임 서버 전용)

### POST `/v1/replay/battles` — 전투 기록 제출

**Request Body**
```json
{
  "account_uid": 1001,
  "battle_type": "dungeon",
  "content_id": 15,
  "duration_seconds": 180,
  "result": "victory",
  "participants": [
    { "uid": 1001, "class": "warrior", "dps": 15000, "damage_taken": 5000 }
  ],
  "replay_data": "base64_encoded_replay_binary"
}
```

**Response 200 OK**
```json
{ "id": 10001 }
```

---

## 공개 엔드포인트 (JWT 인증 필수)

### GET `/v1/replay/battles` — 내 전투 이력

**Headers:** `Authorization: Bearer {token}`

**Query Parameters**

| 파라미터 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `type` | string? | - | `dungeon` / `pvp` / `raid` |
| `page` | int | 1 | 페이지 번호 |

**Response 200 OK**
```json
{
  "battles": [
    {
      "id": 10001,
      "battle_type": "dungeon",
      "content_id": 15,
      "result": "victory",
      "duration_seconds": 180,
      "played_at": "2026-03-29T12:00:00Z"
    }
  ]
}
```

---

### GET `/v1/replay/battles/{id}` — 전투 상세 조회

**Path Parameters:** `id` (long)

**Response 200 OK** — 전투 상세 정보 (본인 전투만 조회 가능)

---

### GET `/v1/replay/battles/{id}/replay` — 리플레이 다운로드

**Path Parameters:** `id` (long)

**Response 200 OK** — 리플레이 데이터 (본인 전투만 다운로드 가능)

**Error Codes**

| 코드 | 설명 |
|---|---|
| 403 | 다른 플레이어의 리플레이 접근 시도 |
| 404 | 리플레이 데이터 없음 (보관 기간 만료 등) |

---

### GET `/v1/replay/stats` — 내 전투 통계

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "account_uid": 1001,
  "total_battles": 250,
  "victories": 180,
  "win_rate": 0.72,
  "avg_dps": 12500,
  "best_dps": 28000,
  "favorite_content": "dungeon"
}
```

---

## 공개 엔드포인트 (인증 불필요)

### GET `/v1/replay/leaderboard` — DPS 리더보드

**Query Parameters**

| 파라미터 | 타입 | 필수 | 설명 |
|---|---|---|---|
| `type` | string | ✓ | `dungeon` / `raid` / `pvp` |
| `content_id` | int | ✓ | 콘텐츠 ID |
| `limit` | int | - | 조회 개수 (기본 10, 최대 100) |

**Response 200 OK**
```json
{
  "leaderboard": [
    {
      "rank": 1,
      "account_uid": 1001,
      "account_name": "홍길동",
      "dps": 28000,
      "battle_id": 10001,
      "played_at": "2026-03-29T12:00:00Z"
    }
  ]
}
```

---

## 관련 문서

- [아키텍처 구조도](../architecture/REPLAY_ARCHITECTURE.md)
- [Ranking API](RANKING_API.md)
