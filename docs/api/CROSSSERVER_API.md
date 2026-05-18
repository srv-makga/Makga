# Cross-Server API

**Base URL:** `http://localhost:5023`  
**Port:** 5023  
**인증 방식:** 내부 네트워크 전용

---

## 개요

크로스서버 서버는 여러 게임 서버 인스턴스 간의 데이터 동기화, 플레이어 이동, 글로벌 이벤트 조율을 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 내부 엔드포인트

### POST `/v1/internal/crossserver/transfer` — 플레이어 서버 이동

플레이어가 한 게임 서버에서 다른 게임 서버로 이동할 때 호출한다.

**Request Body**
```json
{
  "account_uid": 1001,
  "from_server_id": "game-server-01",
  "to_server_id": "game-server-02",
  "target_map_id": 5,
  "target_position": { "x": 100.0, "y": 0.0, "z": 100.0 }
}
```

**Response 200 OK**
```json
{
  "transfer_token": "xfer_abc123",
  "target_server_host": "game-server-02.makga.internal",
  "target_server_port": 9001,
  "expires_at": "2026-03-29T12:05:00Z"
}
```

---

### POST `/v1/internal/crossserver/sync/player` — 플레이어 데이터 동기화

**Request Body**
```json
{
  "account_uid": 1001,
  "data_type": "inventory",
  "data": { ... }
}
```

`data_type`: `inventory` / `stats` / `guild` / `social`

---

### GET `/v1/internal/crossserver/servers` — 활성 서버 목록

**Response 200 OK**
```json
{
  "servers": [
    {
      "server_id": "game-server-01",
      "host": "game-server-01.makga.internal",
      "port": 9001,
      "player_count": 245,
      "max_players": 500,
      "status": "online"
    }
  ]
}
```

---

### POST `/v1/internal/crossserver/broadcast` — 전 서버 이벤트 브로드캐스트

**Request Body**
```json
{
  "event_type": "world_boss_spawn",
  "event_data": { "boss_id": 10, "map_id": 100 }
}
```

**Response 200 OK**
```json
{ "notified_servers": 3 }
```

---

## 관련 문서

- [아키텍처 구조도](../architecture/CROSSSERVER_ARCHITECTURE.md)
- [World API](WORLD_API.md)
