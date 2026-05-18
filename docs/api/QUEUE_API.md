# Queue Server API

**Base URL:** `http://localhost:5003`  
**Port:** 5003  
**인증 방식:** JWT Bearer Token (공개) / 내부 네트워크 전용 (내부)

---

## 개요

대기열 서버는 게임 서버 접속 대기열 관리를 담당한다.  
클라이언트는 대기열에 진입하고 주기적으로 상태를 폴링하여 입장 가능 여부를 확인한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 공개 엔드포인트 (JWT 인증 필수)

### POST `/v1/queue/enter` — 대기열 진입

**Headers:** `Authorization: Bearer {token}`

**Request Body**
```json
{ "server_id": "game-server-01", "world_id": 1 }
```

**Response 200 OK**
```json
{
  "queue_token": "q_abc123",
  "position": 42,
  "estimated_wait_seconds": 120
}
```

---

### GET `/v1/queue/status` — 대기열 상태 조회

**Headers:** `Authorization: Bearer {token}`

**Query Parameters:** `queue_token` (string)

**Response 200 OK**
```json
{
  "status": "waiting",
  "position": 15,
  "estimated_wait_seconds": 45
}
```

`status` 값: `waiting` / `ready` / `expired`

**`ready` 상태 응답 (입장 가능):**
```json
{
  "status": "ready",
  "position": 0,
  "game_server_host": "game-server-01.makga.internal",
  "game_server_port": 9001,
  "session_token": "sess_xyz789"
}
```

---

### DELETE `/v1/queue/leave` — 대기열 이탈

**Headers:** `Authorization: Bearer {token}`

**Query Parameters:** `queue_token` (string)

**Response 200 OK**
```json
{ "message": "대기열에서 이탈했습니다" }
```

---

## 내부 엔드포인트 (내부 네트워크 전용)

### POST `/v1/internal/queue/notify-slot` — 슬롯 해제 알림 (게임 서버 → 대기열 서버)

게임 서버에서 플레이어가 접속 종료 시 호출하여 대기열에 슬롯이 생겼음을 알린다.

**Request Body**
```json
{ "server_id": "game-server-01", "freed_slots": 1 }
```

**Response 200 OK**
```json
{ "promoted_count": 1 }
```

---

## 관련 문서

- [아키텍처 구조도](../architecture/QUEUE_ARCHITECTURE.md)
- [World API](WORLD_API.md)
