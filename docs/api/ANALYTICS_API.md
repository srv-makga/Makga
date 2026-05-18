# Analytics Server API

**Base URL:** `http://localhost:5021`  
**Port:** 5021  
**인증 방식:** 내부 네트워크 전용

---

## 개요

분석 서버는 게임 이벤트 수집, DAU/MAU 집계, 수익 분석, 플레이어 행동 분석을 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 이벤트 수집

### POST `/v1/internal/analytics/event` — 게임 이벤트 수집

**Request Body**
```json
{
  "account_uid": 1001,
  "event_type": "level_up",
  "event_data": {
    "new_level": 50,
    "character_class": "warrior",
    "play_time_seconds": 3600
  },
  "session_id": "sess_abc123",
  "timestamp": "2026-03-29T12:00:00Z"
}
```

**Response 200 OK**

`event_type` 예시: `login` / `logout` / `level_up` / `item_purchase` / `pvp_kill` / `quest_complete` / `dungeon_clear`

---

### POST `/v1/internal/analytics/events/batch` — 배치 이벤트 수집

**Request Body**
```json
{
  "events": [
    { "account_uid": 1001, "event_type": "login", "timestamp": "..." },
    { "account_uid": 1002, "event_type": "level_up", "event_data": {...}, "timestamp": "..." }
  ]
}
```

**Response 200 OK**
```json
{ "received": 2 }
```

---

## 지표 조회

### GET `/v1/internal/analytics/dau` — DAU 조회

**Query Parameters**

| 파라미터 | 타입 | 설명 |
|---|---|---|
| `date` | string | 날짜 (YYYY-MM-DD) |

**Response 200 OK**
```json
{ "date": "2026-03-29", "dau": 15234 }
```

---

### GET `/v1/internal/analytics/retention` — 리텐션 분석

**Query Parameters**

| 파라미터 | 타입 | 설명 |
|---|---|---|
| `cohort_date` | string | 코호트 기준일 (YYYY-MM-DD) |
| `days` | int | 분석 기간 (기본 30) |

**Response 200 OK**
```json
{
  "cohort_date": "2026-03-01",
  "retention": [
    { "day": 1, "rate": 0.65 },
    { "day": 7, "rate": 0.35 },
    { "day": 30, "rate": 0.15 }
  ]
}
```

---

## AppsFlyer S2S 연동

AppsFlyer Server-to-Server (S2S) API를 통해 광고 어트리뷰션이 완료된 사용자의 인앱 이벤트를 서버에서 직접 전송한다.

### POST `/v1/analytics/appsflyer/event` — 단일 이벤트 전송

**Request Body**
```json
{
  "appId": "com.makga.game",
  "advertisingId": "12345678-1234-1234-1234-123456789012",
  "platform": "android",
  "eventName": "af_purchase",
  "eventValues": {
    "af_revenue": 9.99,
    "af_currency": "USD",
    "af_order_id": "ORDER_12345"
  },
  "customerId": "user_1001",
  "ip": "192.168.1.1"
}
```

**Response 200 OK**
```json
{
  "success": true,
  "eventName": "af_purchase",
  "statusCode": 200
}
```

---

### POST `/v1/analytics/appsflyer/batch` — 배치 이벤트 전송

최대 100개의 이벤트를 병렬로 전송한다.

**Request Body**
```json
[
  {
    "advertisingId": "12345678-1234-1234-1234-123456789012",
    "platform": "android",
    "eventName": "af_level_achieved",
    "eventValues": { "af_level": 10 }
  },
  {
    "advertisingId": "87654321-4321-4321-4321-210987654321",
    "platform": "ios",
    "eventName": "af_tutorial_completion",
    "eventValues": { "af_success": "true" }
  }
]
```

**Response 200 OK**
```json
{
  "total": 2,
  "succeeded": 2,
  "failed": 0,
  "results": [
    { "success": true, "eventName": "af_level_achieved", "statusCode": 200 },
    { "success": true, "eventName": "af_tutorial_completion", "statusCode": 200 }
  ]
}
```

---

### POST `/v1/analytics/appsflyer/purchase` — 구매 이벤트 (편의)

**Request Body**
```json
{
  "advertisingId": "12345678-1234-1234-1234-123456789012",
  "platform": "android",
  "revenueUsd": 49.99,
  "orderId": "ORDER_999",
  "contentId": "item_package_01",
  "customerId": "user_1001"
}
```

---

## 관련 문서

- [아키텍처 구조도](../architecture/ANALYTICS_ARCHITECTURE.md)
