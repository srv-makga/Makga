# Mail Server API

**Base URL:** `http://localhost:5010`  
**Port:** 5010  
**인증 방식:** 공개 엔드포인트 — JWT Bearer Token / 내부 엔드포인트 — 내부 네트워크 전용

---

## 개요

우편함 서버는 게임 내 우편 발송, 수신, 첨부 아이템/골드 수령, 우편 삭제를 담당한다.  
아이템/골드 지급 이벤트는 Redis Pub/Sub을 통해 게임 서버에 발행된다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 공개 엔드포인트 (JWT 인증 필수)

### GET `/v1/mail` — 우편함 목록 조회

**Headers:** `Authorization: Bearer {token}`

**Query Parameters**

| 파라미터 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `page` | int | 1 | 페이지 번호 |
| `unread_only` | bool | false | 읽지 않은 우편만 조회 |

**Response 200 OK**
```json
{
  "mails": [
    {
      "mail_id": 1001,
      "sender": "시스템",
      "title": "이벤트 보상",
      "is_read": false,
      "has_attachment": true,
      "created_at": "2026-03-29T12:00:00Z",
      "expires_at": "2026-04-29T12:00:00Z"
    }
  ],
  "total": 5,
  "unread": 3
}
```

---

### GET `/v1/mail/{id}` — 우편 상세 조회

**Path Parameters:** `id` (long)

**Response 200 OK**
```json
{
  "mail_id": 1001,
  "sender": "시스템",
  "title": "이벤트 보상",
  "body": "이벤트 참여 감사합니다!",
  "attachments": [
    { "item_id": 3001, "quantity": 1, "item_name": "전설 검" },
    { "gold": 10000 }
  ],
  "is_read": true,
  "created_at": "2026-03-29T12:00:00Z"
}
```

---

### POST `/v1/mail/{id}/claim` — 첨부물 수령

**Path Parameters:** `id` (long)

**Response 200 OK**
```json
{ "success": true, "claimed_items": [...], "claimed_gold": 10000 }
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 400 | 이미 수령한 우편 |
| 404 | 우편을 찾을 수 없음 |

---

### DELETE `/v1/mail/{id}` — 우편 삭제

**Path Parameters:** `id` (long)

**Response 204 No Content**

**Error Codes**

| 코드 | 설명 |
|---|---|
| 400 | 미수령 첨부물이 있는 우편은 삭제 불가 |

---

## 내부 엔드포인트 (내부 네트워크 전용)

### POST `/v1/internal/mail/send` — 우편 발송 (시스템/GM 전용)

**Request Body**
```json
{
  "recipient_uid": 1001,
  "sender": "시스템",
  "title": "이벤트 보상",
  "body": "이벤트 참여 감사합니다!",
  "attachments": [
    { "item_id": 3001, "quantity": 1 },
    { "gold": 10000 }
  ],
  "expires_days": 30
}
```

**Response 200 OK**
```json
{ "mail_id": 1001 }
```

---

### POST `/v1/internal/mail/bulk-send` — 대량 우편 발송

**Request Body**
```json
{
  "recipient_uids": [1001, 1002, 1003],
  "sender": "시스템",
  "title": "서버 점검 보상",
  "body": "점검 보상입니다.",
  "attachments": [{ "gold": 5000 }],
  "expires_days": 7
}
```

**Response 200 OK**
```json
{ "sent_count": 3, "failed_uids": [] }
```

---

## 관련 문서

- [아키텍처 구조도](../architecture/MAIL_ARCHITECTURE.md)
- [Scheduler API](SCHEDULER_API.md)
