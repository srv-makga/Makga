# Social Server API

**Base URL:** `http://localhost:5009`  
**Port:** 5009  
**인증 방식:** JWT Bearer Token

---

## 개요

소셜 서버는 친구 신청/수락/거절/삭제, 친구 목록 조회, 차단/해제/차단 목록 조회를 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 친구 관리 (JWT 인증 필수)

### POST `/v1/social/friends/request` — 친구 신청

**Headers:** `Authorization: Bearer {token}`

**Request Body**
```json
{
  "to_uid": 1002,
  "to_name": "철수",
  "from_name": "홍길동"
}
```

**Response 200 OK**
```json
{ "message": "친구 신청을 보냈습니다" }
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 400 | 자기 자신에게 신청 / 이미 친구 / 차단된 사용자 |

---

### POST `/v1/social/friends/{uid}/accept` — 친구 신청 수락

**Path Parameters:** `uid` (long) — 신청한 사용자 UID

**Response 200 OK**
```json
{ "message": "친구 신청을 수락했습니다" }
```

---

### DELETE `/v1/social/friends/{uid}/request` — 친구 신청 거절/취소

**Path Parameters:** `uid` (long)

**Response 204 No Content**

---

### DELETE `/v1/social/friends/{uid}` — 친구 삭제

**Path Parameters:** `uid` (long)

**Response 204 No Content**

---

### GET `/v1/social/friends` — 친구 목록 조회

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "friends": [
    {
      "uid": 1002,
      "name": "철수",
      "is_online": true,
      "last_seen": "2026-03-29T12:00:00Z"
    }
  ]
}
```

---

### GET `/v1/social/friends/requests` — 받은 친구 신청 목록

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK** — 대기 중인 친구 신청 목록

---

## 차단 관리 (JWT 인증 필수)

### POST `/v1/social/block/{uid}` — 사용자 차단

**Path Parameters:** `uid` (long)

**Response 200 OK**
```json
{ "message": "user blocked" }
```

---

### DELETE `/v1/social/block/{uid}` — 차단 해제

**Path Parameters:** `uid` (long)

**Response 204 No Content**

---

### GET `/v1/social/block` — 차단 목록 조회

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK** — 차단한 사용자 목록

---

## 내부 엔드포인트

### GET `/v1/internal/social/friends/{uid}` — 친구 목록 조회 (내부 서버용)

게임 서버 등 내부 서버에서 특정 플레이어의 친구 목록을 조회할 때 사용한다.

---

## 관련 문서

- [아키텍처 구조도](../architecture/SOCIAL_ARCHITECTURE.md)
- [Notification API](NOTIFICATION_API.md)
