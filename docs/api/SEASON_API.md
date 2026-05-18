# Season Server API

**Base URL:** `http://localhost:5016`  
**Port:** 5016  
**인증 방식:** JWT Bearer Token (공개) / 내부 네트워크 전용 (내부)

---

## 개요

시즌 서버는 시즌 패스 관리, 경험치 추가, 보상 수령, 프리미엄 패스 구매를 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 공개 엔드포인트 (JWT 인증 필수)

### GET `/v1/season/active` — 현재 활성 시즌 정보

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "id": 5,
  "name": "봄의 전쟁",
  "season_number": 5,
  "start_at": "2026-03-01T00:00:00Z",
  "end_at": "2026-05-31T23:59:59Z",
  "is_active": true,
  "max_level": 100,
  "free_track_count": 10,
  "premium_price": 1500
}
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 404 | 활성 시즌 없음 |

---

### GET `/v1/season/{id}/pass` — 내 시즌 패스 상태

**Path Parameters:** `id` (int) — 시즌 ID

**Response 200 OK**
```json
{
  "season_id": 5,
  "account_uid": 1001,
  "current_level": 23,
  "current_exp": 4500,
  "is_premium": true,
  "unclaimed_rewards": [
    { "level": 20, "reward_type": "item", "item_id": 3001, "quantity": 1 },
    { "level": 25, "reward_type": "gold", "amount": 5000 }
  ]
}
```

---

### POST `/v1/season/{id}/rewards/claim` — 시즌 보상 수령

**Path Parameters:** `id` (int)

**Request Body**
```json
{ "account_uid": 1001, "reward_level": 20 }
```

**Response 200 OK**
```json
{ "success": true, "claimed_reward": { "level": 20, "item_id": 3001, "quantity": 1 } }
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 400 | 이미 수령한 보상 / 레벨 미달 |

---

### POST `/v1/season/{id}/premium/purchase` — 프리미엄 패스 구매

**Path Parameters:** `id` (int)

**Response 200 OK**
```json
{ "success": true, "message": "프리미엄 패스 활성화 완료" }
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 400 | 이미 프리미엄 패스 보유 |

---

## 내부 엔드포인트 (내부 네트워크 전용)

### POST `/v1/internal/season/exp` — 시즌 경험치 추가

퀘스트 완료, 미션 달성 등 게임 서버에서 호출한다.

**Request Body**
```json
{ "account_uid": 1001, "season_id": 5, "exp": 500 }
```

**Response 200 OK**
```json
{
  "new_level": 24,
  "new_exp": 5000,
  "leveled_up": true,
  "new_rewards_available": [{ "level": 25 }]
}
```

---

### POST `/v1/internal/season` — 시즌 생성/업데이트

**Request Body**
```json
{
  "name": "여름의 전쟁",
  "season_number": 6,
  "start_at": "2026-06-01T00:00:00Z",
  "end_at": "2026-08-31T23:59:59Z",
  "max_level": 100,
  "free_track_count": 10,
  "premium_price": 1500
}
```

**Validation:** `start_at`은 반드시 `end_at`보다 이전이어야 한다.

---

## 관련 문서

- [아키텍처 구조도](../architecture/SEASON_ARCHITECTURE.md)
- [Event API](EVENT_API.md)
