# Recommendation Server API

**Base URL:** `http://localhost:5022`  
**Port:** 5022  
**인증 방식:** JWT Bearer Token (공개) / 내부 네트워크 전용 (내부)

---

## 개요

추천 서버는 플레이어 행동 데이터를 기반으로 아이템, 퀘스트, 콘텐츠를 추천한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 추천 엔드포인트 (JWT 인증 필수)

### GET `/v1/recommendation/items` — 아이템 추천

**Headers:** `Authorization: Bearer {token}`

**Query Parameters**

| 파라미터 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `limit` | int | 10 | 추천 개수 |
| `context` | string? | - | 추천 컨텍스트 (`shop` / `gacha` / `craft`) |

**Response 200 OK**
```json
{
  "recommendations": [
    {
      "item_id": 3001,
      "item_name": "전설 검",
      "score": 0.95,
      "reason": "현재 레벨에 최적화된 무기"
    }
  ]
}
```

---

### GET `/v1/recommendation/quests` — 퀘스트 추천

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK** — 플레이어 레벨/진행도에 맞는 퀘스트 추천 목록

---

### GET `/v1/recommendation/content` — 콘텐츠 추천

**Headers:** `Authorization: Bearer {token}`

**Response 200 OK**
```json
{
  "recommendations": [
    {
      "content_type": "dungeon",
      "content_id": 15,
      "name": "어둠의 동굴",
      "reason": "비슷한 레벨의 플레이어들이 즐기는 콘텐츠"
    }
  ]
}
```

---

## 내부 엔드포인트

### POST `/v1/internal/recommendation/feedback` — 추천 피드백 수집

**Request Body**
```json
{
  "account_uid": 1001,
  "recommendation_type": "item",
  "item_id": 3001,
  "action": "purchased",
  "timestamp": "2026-03-29T12:00:00Z"
}
```

`action`: `clicked` / `purchased` / `ignored`

---

## 관련 문서

- [아키텍처 구조도](../architecture/RECOMMENDATION_ARCHITECTURE.md)
- [Analytics API](ANALYTICS_API.md)
