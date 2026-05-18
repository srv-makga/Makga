# GM Server API

**Base URL:** `http://localhost:5008`  
**Port:** 5008  
**인증 방식:** JWT Bearer Token + `gm-role` 정책 (모든 엔드포인트 필수)

---

## 개요

GM 툴 서버는 게임 운영자(GM)가 계정 관리, 밴/해제, 아이템/재화 지급, GM 액션 로그 조회를 수행하는 내부 관리 서버다.  
모든 엔드포인트는 `gm-role` 정책이 적용되어 있어 GM 권한을 가진 JWT만 접근 가능하다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 계정 관리

### GET `/v1/gm/accounts/search` — 계정 검색

**Query Parameters**

| 파라미터 | 타입 | 설명 |
|---|---|---|
| `name` | string? | 계정 이름으로 검색 |
| `email` | string? | 이메일로 검색 |
| `uid` | long? | 계정 UID로 검색 |

**Response 200 OK** — 검색 결과 계정 목록

---

### GET `/v1/gm/accounts/{uid}` — 계정 상세 + 캐릭터 목록

**Path Parameters:** `uid` (long)

**Response 200 OK**
```json
{
  "account": {
    "uid": 1001,
    "email": "user@example.com",
    "name": "홍길동",
    "status": "active",
    "created_at": "2026-01-01T00:00:00Z"
  },
  "characters": [
    { "character_uid": 2001, "name": "길동이", "level": 50, "class": "warrior" }
  ]
}
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 404 | 계정을 찾을 수 없음 |

---

## 밴 관리

### POST `/v1/gm/accounts/{uid}/ban` — 계정 밴

**Path Parameters:** `uid` (long)

**Request Body**
```json
{
  "account_uid": 1001,
  "reason": "핵 사용",
  "duration_days": 30,
  "gm_uid": "gm_001"
}
```

**Response 200 OK**
```json
{ "message": "밴 처리 완료" }
```

---

### DELETE `/v1/gm/accounts/{uid}/ban` — 밴 해제

**Path Parameters:** `uid` (long)  
**Query Parameters:** `gmUid` (string) — GM 식별자

**Response 200 OK**
```json
{ "message": "밴 해제 완료" }
```

---

### GET `/v1/gm/accounts/{uid}/ban-history` — 밴 이력 조회

**Path Parameters:** `uid` (long)

**Response 200 OK** — 밴 이력 목록 배열

---

## 지급 관리

### POST `/v1/gm/accounts/{uid}/grant/item` — 아이템 지급

**Path Parameters:** `uid` (long)

**Request Body**
```json
{
  "account_uid": 1001,
  "item_id": 3001,
  "quantity": 1,
  "reason": "이벤트 보상",
  "gm_uid": "gm_001"
}
```

**Response 200 OK**
```json
{ "success": true, "granted_item_id": 3001, "quantity": 1 }
```

---

### POST `/v1/gm/accounts/{uid}/grant/currency` — 재화 지급

**Path Parameters:** `uid` (long)

**Request Body**
```json
{
  "account_uid": 1001,
  "currency_type": "gold",
  "amount": 100000,
  "reason": "보상 지급",
  "gm_uid": "gm_001"
}
```

**Response 200 OK**
```json
{ "message": "재화 지급 완료" }
```

---

## GM 로그

### GET `/v1/gm/logs` — GM 액션 로그 조회

**Query Parameters**

| 파라미터 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `gmUid` | string? | - | 특정 GM 필터 |
| `page` | int | 1 | 페이지 번호 |

**Response 200 OK** — GM 액션 로그 목록

---

## 관련 문서

- [아키텍처 구조도](../architecture/GM_ARCHITECTURE.md)
- [Report API](REPORT_API.md)
