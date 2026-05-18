# Divine Server API

**Base URL:** `http://localhost:5024`  
**Port:** 5024  
**인증 방식:** JWT Bearer Token (공개) / 내부 네트워크 전용 (내부/관리)

---

## 개요

신 시스템 서버는 플레이어의 신 선택, 신앙 포인트 관리, 신의 개입(축복/저주/진노), 신들의 전쟁을 담당한다.  
게임 세계는 여러 신(GodType)이 존재하며, 플레이어는 하나의 신을 선택하여 신앙 포인트를 쌓고 신의 능력을 받을 수 있다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 공개 엔드포인트 (JWT 인증 필수)

### GET `/v1/divine/gods` — 신 목록 조회

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "gods": [
    {
      "god_type": 1,
      "name": "아레스",
      "domain": "전쟁",
      "description": "전투의 신. 전사 계열 플레이어에게 강력한 버프를 제공한다.",
      "follower_count": 1523,
      "influence_level": 3
    }
  ]
}
```

---

### GET `/v1/divine/my` — 내 신앙 정보 조회

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "account_uid": 1001,
  "god_type": 1,
  "god_name": "아레스",
  "faith_points": 2500,
  "faith_rank": "신도",
  "active_blessings": [
    { "blessing_id": 1, "name": "전투의 축복", "expires_at": "2026-03-30T12:00:00Z" }
  ]
}
```

---

### POST `/v1/divine/choose` — 신 선택

**Request Body**
```json
{ "god_type": 1 }
```

**Response 200 OK**
```json
{ "message": "아레스를 선택했습니다.", "initial_faith_points": 100 }
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 400 | 이미 신을 선택한 상태 (변경 쿨다운 적용) |

---

### POST `/v1/divine/pray` — 기도 (신앙 포인트 획득)

**Response 200 OK**
```json
{ "gained_faith": 50, "total_faith": 2550, "next_pray_at": "2026-03-29T18:00:00Z" }
```

---

### GET `/v1/divine/blessings` — 사용 가능한 축복 목록

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK** — 현재 신앙 포인트로 사용 가능한 축복 목록

---

### POST `/v1/divine/blessings/{id}/activate` — 축복 활성화

**Path Parameters:** `id` (int) — 축복 ID

**Response 200 OK**
```json
{ "success": true, "consumed_faith": 500, "remaining_faith": 2050 }
```

---

## 내부/관리 엔드포인트

### POST `/v1/internal/divine/influence` — 신의 영향력 활성화

**Request Body**
```json
{
  "god_type": 1,
  "influence_level": 3,
  "domain_map_id": 5
}
```

**Response 200 OK**
```json
{ "message": "아레스의 영향력이 활성화되었습니다." }
```

---

### POST `/v1/internal/divine/wrath/{map_id}` — 신의 진노 발동

**Path Parameters:** `map_id` (int)

**Request Body**
```json
{ "god_type": 1 }
```

**Response 200 OK**
```json
{ "message": "맵 5에 아레스 진노 발동 (30초)" }
```

---

### POST `/v1/internal/divine/war/start` — 신들의 전쟁 시작

**Request Body**
```json
{ "participating_gods": [1, 2, 3], "duration_hours": 2 }
```

---

### POST `/v1/internal/divine/war/end/{winning_god}` — 신들의 전쟁 종료

**Path Parameters:** `winning_god` (int) — 승리한 신의 GodType

---

### GET `/v1/internal/divine/war/status` — 신들의 전쟁 현황

**Response 200 OK**
```json
{
  "is_active": true,
  "start_at": "2026-03-29T20:00:00Z",
  "end_at": "2026-03-29T22:00:00Z",
  "scores": [
    { "god_type": 1, "name": "아레스", "score": 15000 },
    { "god_type": 2, "name": "아테나", "score": 12000 }
  ]
}
```

---

### GET `/v1/internal/divine/interventions` — 최근 신의 개입 이력

**Response 200 OK** — 최근 50건의 신의 개입 이력

---

## 관련 문서

- [아키텍처 구조도](../architecture/DIVINE_ARCHITECTURE.md)
- [Game Server 구조도](../architecture/GAME_SERVER_ARCHITECTURE.md)
