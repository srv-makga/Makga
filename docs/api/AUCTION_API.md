# Auction Server API

**Base URL:** `http://localhost:5004`  
**Port:** 5004  
**인증 방식:** JWT Bearer Token

---

## 개요

경매장 서버는 아이템 등록, 구매, 즉시 구매, 등록 취소, 검색, 내 경매 목록 조회를 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 경매장 엔드포인트 (JWT 인증 필수)

### GET `/v1/auction/items` — 경매장 아이템 검색

**Headers:** `Authorization: Bearer {token}`

**Query Parameters**

| 파라미터 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `category` | string? | - | 아이템 카테고리 필터 |
| `min_price` | long? | - | 최소 가격 |
| `max_price` | long? | - | 최대 가격 |
| `sort` | string | `price_asc` | 정렬 방식 (`price_asc` / `price_desc` / `newest`) |
| `page` | int | 1 | 페이지 번호 |

**Response 200 OK**
```json
{
  "items": [
    {
      "auction_id": 5001,
      "item_id": 3001,
      "item_name": "전설 검",
      "seller_uid": 1001,
      "seller_name": "홍길동",
      "start_price": 100000,
      "buyout_price": 500000,
      "current_bid": 150000,
      "bid_count": 3,
      "expires_at": "2026-03-30T12:00:00Z"
    }
  ],
  "total": 42,
  "page": 1
}
```

---

### POST `/v1/auction/items` — 아이템 경매 등록

**Headers:** `Authorization: Bearer {token}`

**Request Body**
```json
{
  "item_uid": 9001,
  "start_price": 100000,
  "buyout_price": 500000,
  "duration_hours": 24
}
```

**Response 200 OK**
```json
{ "auction_id": 5001 }
```

---

### POST `/v1/auction/items/{id}/bid` — 입찰

**Path Parameters:** `id` (long) — 경매 ID

**Request Body**
```json
{ "bid_amount": 200000 }
```

**Response 200 OK**
```json
{ "success": true, "current_bid": 200000 }
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 400 | 현재 최고 입찰가보다 낮은 금액 |
| 404 | 경매를 찾을 수 없음 또는 종료됨 |

---

### POST `/v1/auction/items/{id}/buyout` — 즉시 구매

**Path Parameters:** `id` (long)

**Response 200 OK**
```json
{ "success": true, "paid": 500000 }
```

---

### DELETE `/v1/auction/items/{id}` — 경매 취소

본인이 등록한 경매만 취소 가능하다. 입찰이 있는 경우 취소 불가.

**Path Parameters:** `id` (long)

**Response 200 OK**
```json
{ "success": true, "message": "경매 취소 완료" }
```

---

### GET `/v1/auction/my` — 내 경매 목록

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK** — 내가 등록한 경매 목록

---

### GET `/v1/auction/my/bids` — 내 입찰 목록

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK** — 내가 입찰한 경매 목록

---

## 관련 문서

- [아키텍처 구조도](../architecture/AUCTION_ARCHITECTURE.md)
- [Billing API](BILLING_API.md)
