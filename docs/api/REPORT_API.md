# Report Server API

**Base URL:** `http://localhost:5013`  
**Port:** 5013  
**인증 방식:** JWT Bearer Token (신고 제출) / 내부 네트워크 전용 (관리)

---

## 개요

신고/제재 서버는 플레이어 신고 접수, 신고 관리, 밴 발급/해제를 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 공개 엔드포인트

### POST `/v1/report` — 신고 제출 (JWT 인증 필수)

**Headers:** `Authorization: Bearer {token}`

**Request Body**
```json
{
  "target_uid": 1002,
  "reason": "욕설 및 비매너",
  "detail": "채팅에서 지속적인 욕설을 사용했습니다.",
  "evidence_screenshot_url": "https://..."
}
```

**Response 200 OK**
```json
{ "report_id": 8001 }
```

**Error Codes**

| 코드 | 설명 |
|---|---|
| 429 | 신고 횟수 초과 (Rate Limit) |

---

## 내부 엔드포인트 (GM/내부 전용)

### GET `/v1/internal/reports` — 신고 목록 조회

**Query Parameters**

| 파라미터 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `status` | string? | - | `pending` / `reviewed` / `resolved` |
| `page` | int | 0 | 페이지 번호 |

**Response 200 OK** — 신고 목록

---

### GET `/v1/internal/reports/{id}` — 신고 단건 조회

**Path Parameters:** `id` (long)

**Response 200 OK** — 신고 상세 정보

---

### PUT `/v1/internal/reports/{id}` — 신고 상태 업데이트

**Path Parameters:** `id` (long)

**Request Body**
```json
{
  "status": "resolved",
  "gm_note": "경고 처리 완료",
  "gm_uid": "gm_001"
}
```

---

### GET `/v1/internal/accounts/{uid}/reports` — 플레이어 신고 내역

**Path Parameters:** `uid` (long)

**Response 200 OK** — 해당 플레이어에 대한 신고 목록

---

### GET `/v1/internal/accounts/{uid}/ban` — 활성 밴 조회

**Path Parameters:** `uid` (long)

**Response 200 OK** — 현재 활성 밴 정보

---

### POST `/v1/internal/accounts/{uid}/ban` — 밴 발급

**Path Parameters:** `uid` (long)

**Request Body**
```json
{
  "account_uid": 1002,
  "reason": "핵 사용",
  "duration_days": 30,
  "gm_uid": "gm_001"
}
```

**Response 200 OK**
```json
{ "ban_id": 9001 }
```

---

### DELETE `/v1/internal/accounts/{uid}/ban/{ban_id}` — 밴 해제

**Path Parameters:** `uid` (long), `ban_id` (long)

**Query Parameters:** `gmUid` (string)

**Response 200 OK**
```json
{ "message": "밴 해제 완료" }
```

---

## 관련 문서

- [아키텍처 구조도](../architecture/REPORT_ARCHITECTURE.md)
- [GM API](GM_API.md)
