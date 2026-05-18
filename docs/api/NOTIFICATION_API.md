# Notification Server API

**Base URL:** `http://localhost:5011`  
**Port:** 5011  
**인증 방식:** JWT Bearer Token (공개) / 내부 네트워크 전용 (내부)

---

## 개요

알림 서버는 푸시 알림 발송, 알림 목록 조회, 읽음 처리를 담당한다.  
FCM(Firebase Cloud Messaging)을 통해 모바일 푸시 알림을 전송한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 공개 엔드포인트 (JWT 인증 필수)

### POST `/v1/notification/device` — 디바이스 토큰 등록

**Headers:** `Authorization: Bearer {token}`

**Request Body**
```json
{
  "device_token": "fcm_device_token_string",
  "platform": "android",
  "app_version": "1.0.0"
}
```

**Response 200 OK**
```json
{ "message": "디바이스 토큰 등록 완료" }
```

---

### DELETE `/v1/notification/device` — 디바이스 토큰 삭제 (로그아웃)

**Headers:** `Authorization: Bearer {token}`

**Query Parameters:** `device_token` (string)

---

### GET `/v1/notification` — 알림 목록 조회

**Headers:** `Authorization: Bearer {token}`

**Query Parameters**

| 파라미터 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `page` | int | 1 | 페이지 번호 |
| `unread_only` | bool | false | 읽지 않은 알림만 |

**Response 200 OK**
```json
{
  "notifications": [
    {
      "id": 6001,
      "type": "friend_request",
      "title": "친구 신청",
      "body": "철수님이 친구 신청을 보냈습니다.",
      "is_read": false,
      "created_at": "2026-03-29T12:00:00Z"
    }
  ],
  "unread_count": 3
}
```

---

### POST `/v1/notification/{id}/read` — 알림 읽음 처리

**Path Parameters:** `id` (long)

**Response 200 OK**

---

### POST `/v1/notification/read-all` — 전체 읽음 처리

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{ "updated_count": 3 }
```

---

## 내부 엔드포인트 (내부 네트워크 전용)

### POST `/v1/internal/notification/send` — 푸시 알림 발송

**Request Body**
```json
{
  "recipient_uid": 1001,
  "type": "friend_request",
  "title": "친구 신청",
  "body": "철수님이 친구 신청을 보냈습니다.",
  "data": { "from_uid": 1002 }
}
```

**Response 200 OK**
```json
{ "notification_id": 6001, "push_sent": true }
```

---

### POST `/v1/internal/notification/broadcast` — 전체 방송 알림

**Request Body**
```json
{
  "type": "server_maintenance",
  "title": "서버 점검 안내",
  "body": "오늘 오전 2시부터 4시까지 서버 점검이 있습니다.",
  "target_segment": "all"
}
```

---

## 관련 문서

- [아키텍처 구조도](../architecture/NOTIFICATION_ARCHITECTURE.md)
- [Social API](SOCIAL_API.md)
