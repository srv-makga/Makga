# Housing Server API

**Base URL:** `http://localhost:5014`  
**Port:** 5014  
**인증 방식:** JWT Bearer Token

---

## 개요

하우징 서버는 플레이어 주택 조회, 가구 배치/이동/삭제, 방문자 관리를 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 주택 관리 (JWT 인증 필수)

### GET `/v1/housing/my` — 내 주택 조회

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "house_id": 7001,
  "owner_uid": 1001,
  "name": "홍길동의 집",
  "theme": "medieval",
  "size": "medium",
  "furniture": [
    {
      "furniture_id": 1,
      "item_id": 4001,
      "item_name": "나무 침대",
      "position": { "x": 3, "y": 0, "z": 2 },
      "rotation": 0
    }
  ],
  "visitor_count_today": 12
}
```

---

### GET `/v1/housing/{house_id}` — 특정 주택 조회 (방문)

**Path Parameters:** `house_id` (long)

**Response 200 OK** — 주택 정보 (가구 배치 포함)

---

### PUT `/v1/housing/my/name` — 주택 이름 변경

**Request Body**
```json
{ "name": "새로운 집 이름" }
```

---

### POST `/v1/housing/my/furniture` — 가구 배치

**Request Body**
```json
{
  "item_id": 4001,
  "position": { "x": 3, "y": 0, "z": 2 },
  "rotation": 90
}
```

**Response 200 OK**
```json
{ "furniture_id": 2, "message": "가구 배치 완료" }
```

---

### PUT `/v1/housing/my/furniture/{id}` — 가구 이동/회전

**Path Parameters:** `id` (int) — 가구 인스턴스 ID

**Request Body**
```json
{
  "position": { "x": 5, "y": 0, "z": 3 },
  "rotation": 0
}
```

---

### DELETE `/v1/housing/my/furniture/{id}` — 가구 제거

**Path Parameters:** `id` (int)

**Response 200 OK**
```json
{ "message": "가구 제거 완료" }
```

---

### GET `/v1/housing/popular` — 인기 주택 목록

**Query Parameters**

| 파라미터 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `limit` | int | 10 | 조회 개수 |

**Response 200 OK** — 방문자 수 기준 인기 주택 목록

---

## 관련 문서

- [아키텍처 구조도](../architecture/HOUSING_ARCHITECTURE.md)
