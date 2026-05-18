# Scheduler Server API

**Base URL:** `http://localhost:5019`  
**Port:** 5019  
**인증 방식:** 내부 네트워크 전용

---

## 개요

스케줄러 서버는 주기적인 게임 이벤트(출석 보상, 랭킹 정산, 시즌 종료 등)를 자동으로 실행하고 이벤트를 발행한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 스케줄 관리 (내부 전용)

### GET `/v1/internal/scheduler/jobs` — 등록된 스케줄 목록

**Response 200 OK**
```json
{
  "jobs": [
    {
      "job_id": "daily_attendance",
      "cron": "0 0 * * *",
      "last_run": "2026-03-29T00:00:00Z",
      "next_run": "2026-03-30T00:00:00Z",
      "status": "active"
    }
  ]
}
```

---

### POST `/v1/internal/scheduler/jobs/{id}/trigger` — 스케줄 즉시 실행

**Path Parameters:** `id` (string) — 잡 ID

**Response 200 OK**
```json
{ "message": "job triggered", "job_id": "daily_attendance" }
```

---

### PUT `/v1/internal/scheduler/jobs/{id}/pause` — 스케줄 일시 정지

**Path Parameters:** `id` (string)

---

### PUT `/v1/internal/scheduler/jobs/{id}/resume` — 스케줄 재개

**Path Parameters:** `id` (string)

---

## 이벤트 발행 (EventPublisherService)

스케줄러는 다음 이벤트를 Redis Pub/Sub 채널에 발행한다:

| 이벤트 | 채널 | 주기 |
|---|---|---|
| 일일 출석 보상 | `event:daily_attendance` | 매일 00:00 |
| 랭킹 주간 정산 | `event:ranking_weekly` | 매주 월요일 00:00 |
| 시즌 종료 처리 | `event:season_end` | 시즌 종료 시 |
| 월드 보스 소환 | `event:world_boss_spawn` | 설정된 시간 |
| 경매장 만료 처리 | `event:auction_expire` | 매시간 |

---

## 관련 문서

- [아키텍처 구조도](../architecture/SCHEDULER_ARCHITECTURE.md)
- [Event API](EVENT_API.md)
