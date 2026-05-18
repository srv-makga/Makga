# Event Server API

**Base URL:** `http://localhost:5015`  
**Port:** 5015  
**인증 방식:** JWT Bearer Token (공개) / 내부 네트워크 전용 (내부)

---

## 개요

이벤트 서버는 게임 이벤트 목록 조회, 이벤트 참여, 보상 수령, 가챠(뽑기), 강화 시뮬레이션을 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 이벤트 엔드포인트 (JWT 인증 필수)

### GET `/v1/event/active` — 진행 중인 이벤트 목록

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "events": [
    {
      "event_id": 101,
      "name": "봄맞이 이벤트",
      "type": "attendance",
      "start_at": "2026-03-01T00:00:00Z",
      "end_at": "2026-03-31T23:59:59Z",
      "description": "매일 출석하고 보상을 받으세요!"
    }
  ]
}
```

---

### GET `/v1/event/{id}` — 이벤트 상세 조회

**Path Parameters:** `id` (int)

**Response 200 OK** — 이벤트 상세 정보 + 내 참여 현황

---

### POST `/v1/event/{id}/participate` — 이벤트 참여

**Path Parameters:** `id` (int)

**Response 200 OK**
```json
{ "success": true, "progress": 1, "max_progress": 7 }
```

---

### POST `/v1/event/{id}/rewards/claim` — 이벤트 보상 수령

**Path Parameters:** `id` (int)

**Request Body**
```json
{ "reward_step": 3 }
```

**Response 200 OK**
```json
{ "success": true, "rewards": [{ "item_id": 3001, "quantity": 1 }] }
```

---

## 가챠 (뽑기)

### GET `/v1/event/gacha/{banner_id}` — 가챠 배너 정보 조회

**Path Parameters:** `banner_id` (int)

**Response 200 OK** — 배너 정보, 등급별 확률, 천장(Pity) 카운트

---

### POST `/v1/event/gacha/{banner_id}/draw` — 가챠 실행

**Path Parameters:** `banner_id` (int)

**Request Body**
```json
{ "count": 10 }
```

`count`: `1` 또는 `10`

**Response 200 OK**
```json
{
  "results": [
    { "item_id": 3001, "item_name": "전설 검", "grade": "S", "is_new": true }
  ],
  "pity_count": 45,
  "used_currency": 1500
}
```

---

## 강화 시뮬레이션

### GET `/v1/event/enhance/tables` — 강화 확률 테이블 조회

**Response 200 OK** — 아이템 유형별 강화 확률 테이블

강화 결과: `success`(성공) / `downgrade`(하락) / `break`(파괴) / `maintain`(유지)

---

## 관련 문서

- [아키텍처 구조도](../architecture/EVENT_ARCHITECTURE.md)
- [Season API](SEASON_API.md)
