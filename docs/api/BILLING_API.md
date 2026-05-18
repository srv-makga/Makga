# Billing Server API

**Base URL:** `http://localhost:5005`  
**Port:** 5005  
**인증 방식:** JWT Bearer Token (공개 엔드포인트) / 내부 네트워크 전용 (내부 엔드포인트)

---

## 개요

결제 서버는 인앱 결제(IAP) 검증, 지갑 잔액 관리, 구독 관리, 원장 이력 조회를 담당한다.  
공개 엔드포인트는 `/v1/billing` 경로, 내부 엔드포인트는 `/v1/internal/billing` 경로를 사용한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 공개 엔드포인트 (JWT 인증 필수)

### GET `/v1/billing/wallet` — 지갑 잔액 조회

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "account_uid": 1001,
  "balance": 15000,
  "checked_at": "2026-03-29T12:00:00Z"
}
```

---

### POST `/v1/billing/charge` — IAP 충전

Google Play / Apple App Store / One Store 영수증을 검증하고 캐시를 충전한다.

**Headers:** `Authorization: Bearer {token}`

**Request Body**
```json
{
  "account_uid": 1001,
  "platform": "google",
  "product_id": "cash_5000",
  "purchase_token": "google_purchase_token",
  "transaction_id": null,
  "receipt_data": null
}
```

| 필드 | 타입 | 설명 |
|---|---|---|
| `platform` | string | `google` / `apple` / `onestore` |
| `product_id` | string | 스토어 상품 ID |
| `purchase_token` | string? | Google Play 구매 토큰 |
| `transaction_id` | string? | Apple 트랜잭션 ID |
| `receipt_data` | string? | Apple 영수증 데이터 (Base64) |

**Response 200 OK**
```json
{
  "success": true,
  "ledger_id": 5001,
  "amount": 5000,
  "new_balance": 20000
}
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 400 | 영수증 검증 실패 또는 이미 처리된 영수증 |
| 401 | JWT 인증 실패 |
| 403 | JWT uid와 body uid 불일치 |

---

### GET `/v1/billing/history` — 원장 이력 조회

**Headers:** `Authorization: Bearer {token}`

**Query Parameters**

| 파라미터 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `page` | int | 1 | 페이지 번호 (1부터 시작) |

**Response 200 OK** — 원장 이력 목록 배열

---

### GET `/v1/billing/subscription` — 구독 정보 조회

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "subscription": { "tier": "premium", "expires_at": "2026-04-29T12:00:00Z" },
  "tier": "premium",
  "benefits": {
    "daily_bonus": 100,
    "exp_multiplier": 1.5,
    "storage_slots": 200
  }
}
```

---

### GET `/v1/billing/subscription/benefits/{tier}` — 티어 혜택 조회 (공개)

**Path Parameters:** `tier` — `free` / `basic` / `premium` / `vip`

**Response 200 OK** — 해당 티어의 혜택 정보

---

## 내부 엔드포인트 (내부 네트워크 전용, 인증 없음)

### POST `/v1/internal/billing/grant` — GM 캐시 지급

**Request Body**
```json
{
  "account_uid": 1001,
  "amount": 10000,
  "description": "이벤트 보상 지급"
}
```

**Response 200 OK**
```json
{
  "ledger_id": 5002,
  "new_balance": 25000
}
```

**Validation**
- `account_uid`와 `amount`는 반드시 양수여야 한다.

---

## 관련 문서

- [아키텍처 구조도](../architecture/BILLING_ARCHITECTURE.md)
- [GM API](GM_API.md)
