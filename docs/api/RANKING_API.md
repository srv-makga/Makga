# Ranking Server API Documentation

**Base URL:** `http://localhost:5001/v1/ranking`  
**Port:** 5001

---

## Overview

Ranking Server는 Redis 기반 리더보드 시스템을 제공합니다. 점수 업데이트, 상위 순위 조회, 개별 사용자 순위 조회 등의 기능을 지원합니다.

**Key Features:**
- 다중 리더보드 지원 (예: 주간, 월간, 전체 랭킹)
- 상위 순위 캐싱 (TTL 설정 가능)
- 실시간 점수 업데이트
- 개별 사용자 순위 조회

---

## 1. Score Management

### 1.1 점수 업데이트
**POST** `/{leaderboard}/score`

**Description:** 특정 리더보드에 사용자의 점수를 업데이트합니다. 같은 사용자의 기존 점수가 있으면 덮어씁니다.

**Request:**
```json
{
  "userId": "player123",
  "score": 1500
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| userId | string | ✓ | 사용자 고유 ID |
| score | number | ✓ | 점수 (double 타입, 소수점 지원) |

**URL Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| leaderboard | string | ✓ | 리더보드 이름 (예: weekly, monthly, total) |

**Response (200 OK):**
```json
{}
```

**Error Responses:**
- `400 Bad Request` — userId/score 누락 또는 invalid
- `500 Internal Server Error` — Redis 연결 오류

**Example:**
```bash
curl -X POST http://localhost:5001/v1/ranking/weekly/score \
  -H "Content-Type: application/json" \
  -d '{"userId":"player123","score":1500}'
```

---

## 2. Leaderboard Queries

### 2.1 상위 순위 조회
**GET** `/{leaderboard}/top?count=10`

**Description:** 특정 리더보드의 상위 순위 사용자들을 조회합니다.

**Query Parameters:**
| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| count | int | ✗ | 10 | 반환할 상위 순위 개수 (1-100 권장) |

**URL Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| leaderboard | string | ✓ | 리더보드 이름 (예: weekly, monthly, total) |

**Response (200 OK):**
```json
[
  {
    "rank": 1,
    "userId": "player123",
    "score": 5000,
    "timestamp": "2024-03-07T10:30:00Z"
  },
  {
    "rank": 2,
    "userId": "player456",
    "score": 4800,
    "timestamp": "2024-03-07T09:15:00Z"
  },
  {
    "rank": 3,
    "userId": "player789",
    "score": 4600,
    "timestamp": "2024-03-07T08:45:00Z"
  }
]
```

**Caching:** 응답은 TTL(Time-To-Live)에 따라 캐시됩니다. 같은 `leaderboard`와 `count` 조합으로 호출하면 캐시된 데이터를 반환합니다.

**Error Responses:**
- `500 Internal Server Error` — Redis 연결 오류

**Example:**
```bash
# 주간 랭킹 상위 20명 조회
curl http://localhost:5001/v1/ranking/weekly/top?count=20

# 기본값(10명)으로 월간 랭킹 조회
curl http://localhost:5001/v1/ranking/monthly/top
```

---

### 2.2 개별 사용자 순위 조회
**GET** `/{leaderboard}/user/{userId}`

**Description:** 특정 리더보드에서 사용자의 현재 순위와 점수를 조회합니다.

**URL Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| leaderboard | string | ✓ | 리더보드 이름 (예: weekly, monthly, total) |
| userId | string | ✓ | 사용자 고유 ID |

**Response (200 OK):**
```json
{
  "rank": 15,
  "userId": "player123",
  "score": 3500,
  "timestamp": "2024-03-07T10:30:00Z"
}
```

**Error Responses:**
- `404 Not Found` — 사용자가 해당 리더보드에 없음 (아직 점수를 제출하지 않았거나, 삭제됨)
- `500 Internal Server Error` — Redis 연결 오류

**Example:**
```bash
# 주간 랭킹에서 player123의 순위 조회
curl http://localhost:5001/v1/ranking/weekly/user/player123
```

---

## Response Models

### RankingEntry
순위 정보를 담는 응답 모델:

```json
{
  "rank": 1,
  "userId": "player123",
  "score": 5000,
  "timestamp": "2024-03-07T10:30:00Z"
}
```

**Fields:**
| Field | Type | Description |
|-------|------|-------------|
| rank | int | 순위 (1부터 시작) |
| userId | string | 사용자 고유 ID |
| score | double | 현재 점수 |
| timestamp | string | 마지막 점수 업데이트 시간 (ISO 8601) |

---

## Leaderboard Naming Conventions

리더보드 이름은 자유롭게 정의할 수 있으나, 권장 네이밍:

| Name | Usage | Description |
|------|-------|-------------|
| `daily` | 일간 랭킹 | 매일 00:00 UTC 초기화 |
| `weekly` | 주간 랭킹 | 매주 월요일 00:00 UTC 초기화 |
| `monthly` | 월간 랭킹 | 매월 1일 00:00 UTC 초기화 |
| `total` | 전체 랭킹 | 초기화 없음 |
| `pvp` | PvP 레이팅 | 전투 승리 기반 |
| `pve` | PvE 점수 | 던전 클리어 기반 |

---

## Implementation Details

### Redis Storage
- **Data Structure:** Redis Sorted Set (점수 기반 정렬)
- **Key Format:** `leaderboard:{leaderboard_name}`
- **Member:** `{userId}` (점수는 score로 저장)

### Scoring System
- **Score Type:** Double (부동소수점, 소수점 이하 점수 지원)
- **Sorting Order:** 내림차순 (높은 점수가 상위)
- **Duplicate Updates:** 같은 사용자 ID의 점수는 덮어쓰기 (최신 점수 유지)

### Performance
- **Get Top:** O(log N + count) — Redis ZREVRANGE 사용
- **Get User Rank:** O(log N) — Redis ZREVRANK 사용
- **Set Score:** O(log N) — Redis ZADD 사용

---

## Usage Examples

### cURL 예제

**주간 랭킹에 점수 제출:**
```bash
curl -X POST http://localhost:5001/v1/ranking/weekly/score \
  -H "Content-Type: application/json" \
  -d '{
    "userId": "player123",
    "score": 2500
  }'
```

**전체 랭킹 상위 50명 조회:**
```bash
curl "http://localhost:5001/v1/ranking/total/top?count=50"
```

**월간 랭킹에서 특정 사용자 순위 조회:**
```bash
curl "http://localhost:5001/v1/ranking/monthly/user/player456"
```

### Client 예제 (JavaScript)

```javascript
// 점수 업데이트
async function submitScore(leaderboard, userId, score) {
  const response = await fetch(
    `http://localhost:5001/v1/ranking/${leaderboard}/score`,
    {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ userId, score })
    }
  );
  return response.ok;
}

// 상위 10명 조회
async function getTopRankings(leaderboard) {
  const response = await fetch(
    `http://localhost:5001/v1/ranking/${leaderboard}/top?count=10`
  );
  return response.json();
}

// 사용자 순위 조회
async function getUserRank(leaderboard, userId) {
  const response = await fetch(
    `http://localhost:5001/v1/ranking/${leaderboard}/user/${userId}`
  );
  if (response.status === 404) return null;
  return response.json();
}
```

---

## Caching Strategy

상위 순위(`/top`) 엔드포인트는 응답이 캐시되므로, 자주 변경되지 않는 데이터에 최적화되어 있습니다.

**Cache Key:** `{leaderboard}:{count}`  
**Default TTL:** 설정에 따라 결정 (예: 60초)

Real-time 점수가 필요한 경우 개별 사용자 순위(`/user/{userId}`) 조회를 권장합니다.

---

## Error Handling

| HTTP Status | Error | Description |
|-------------|-------|-------------|
| 200 | N/A | 성공 |
| 400 | Bad Request | 요청 파라미터 누락 또는 invalid |
| 404 | Not Found | 사용자가 리더보드에 없음 |
| 500 | Internal Server Error | 서버 오류 (Redis 연결 실패 등) |

---

## Related Documentation

- [Login Server API](./LOGIN_API.md) — 인증 및 토큰 관리
- [World Server API](./WORLD_API.md) — 플레이어 및 서버 관리
