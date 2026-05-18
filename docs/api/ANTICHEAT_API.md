# Anti-Cheat Server API

**Base URL:** `http://localhost:5020`  
**Port:** 5020  
**인증 방식:** 내부 네트워크 전용 (게임 서버 → 안티치트 서버)

---

## 개요

안티치트 서버는 게임 서버에서 수집한 플레이어 행동 데이터를 분석하여 이상 행동을 탐지하고 자동 제재를 처리한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 내부 엔드포인트 (게임 서버 전용)

### POST `/v1/internal/anticheat/report` — 이상 행동 보고

게임 서버가 의심스러운 플레이어 행동을 감지했을 때 호출한다.

**Request Body**
```json
{
  "account_uid": 1001,
  "violation_type": "speed_hack",
  "severity": "high",
  "evidence": {
    "position_before": { "x": 100, "y": 0, "z": 100 },
    "position_after": { "x": 5000, "y": 0, "z": 5000 },
    "time_elapsed_ms": 100,
    "expected_max_distance": 50
  },
  "server_id": "game-server-01",
  "timestamp": "2026-03-29T12:00:00Z"
}
```

**Response 200 OK**
```json
{
  "violation_id": 7001,
  "action_taken": "warning",
  "auto_ban": false
}
```

`violation_type`: `speed_hack` / `teleport` / `damage_hack` / `item_dupe` / `bot_behavior`  
`action_taken`: `warning` / `kick` / `temp_ban` / `permanent_ban`

---

### GET `/v1/internal/anticheat/violations/{uid}` — 위반 이력 조회

**Path Parameters:** `uid` (long)

**Response 200 OK**
```json
{
  "account_uid": 1001,
  "total_violations": 3,
  "violations": [
    {
      "violation_id": 7001,
      "type": "speed_hack",
      "severity": "high",
      "action": "warning",
      "timestamp": "2026-03-29T12:00:00Z"
    }
  ]
}
```

---

### POST `/v1/internal/anticheat/validate-action` — 게임 액션 검증

게임 서버가 플레이어 액션의 유효성을 실시간으로 검증할 때 사용한다.

**Request Body**
```json
{
  "account_uid": 1001,
  "action_type": "item_acquire",
  "action_data": {
    "item_id": 3001,
    "quantity": 1,
    "source": "monster_drop"
  }
}
```

**Response 200 OK**
```json
{ "valid": true, "risk_score": 0.05 }
```

`risk_score`: 0.0(정상) ~ 1.0(매우 의심)

---

## 관련 문서

- [아키텍처 구조도](../architecture/ANTICHEAT_ARCHITECTURE.md)
- [Report API](REPORT_API.md)
