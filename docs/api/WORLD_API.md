# World Server API Documentation

**Base URL:** `http://localhost:5002/v1/internal`  
**Port:** 5002  
**Visibility:** Internal (game servers only)

---

## Overview

World Server는 게임 서버 관리 및 플레이어 세션 추적을 담당합니다. 주요 기능:

- **Server Registration** — 게임 서버 등록 및 하트비트 (헬스 체크)
- **Player Session Management** — 플레이어 입장/퇴장 추적
- **Server Discovery** — 클라이언트를 적절한 게임 서버로 라우팅

**Internal API 주의:** 이 API는 내부 서버 간 통신용으로 설계되었으며, 클라이언트 직접 접근은 권장되지 않습니다.

---

## 1. Server Management

### 1.1 게임 서버 등록
**POST** `/server/register`

**Description:** 새로운 게임 서버를 World Server에 등록합니다. 서버의 기본 정보(ID, 월드, 호스트, 포트, 최대 플레이어 수)를 저장합니다.

**Request:**
```json
{
  "serverId": "game-server-01",
  "worldId": "world-1",
  "mapId": "map-dungeon-1",
  "host": "192.168.1.100",
  "port": 9001,
  "maxPlayers": 100
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| serverId | string | ✓ | 게임 서버 고유 ID (예: game-server-01) |
| worldId | string | ✓ | 월드 ID (게임 세계 식별자) |
| mapId | string | ✗ | 맵 ID (선택사항, 멀티맵 구조용) |
| host | string | ✓ | 서버 IP 또는 호스트명 |
| port | int | ✓ | 클라이언트 연결 포트 |
| maxPlayers | int | ✓ | 최대 동시 플레이어 수 |

**Response (200 OK):**
```json
{
  "message": "Server registered successfully"
}
```

**Error Responses:**
- `400 Bad Request` — 필수 파라미터 누락
- `500 Internal Server Error` — 서버 등록 실패

**Example:**
```bash
curl -X POST http://localhost:5002/v1/internal/server/register \
  -H "Content-Type: application/json" \
  -d '{
    "serverId": "game-server-01",
    "worldId": "world-1",
    "mapId": "map-town-1",
    "host": "192.168.1.100",
    "port": 9001,
    "maxPlayers": 100
  }'
```

---

### 1.2 서버 하트비트 (헬스 체크)
**PUT** `/server/heartbeat`

**Description:** 게임 서버가 살아있음을 주기적으로 알립니다. 일반적으로 10-30초마다 호출됩니다.

**주의:** 일정 시간 동안 하트비트를 받지 못하면 서버가 오프라인 상태로 표시됩니다.

**Request:**
```json
{
  "serverId": "game-server-01"
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| serverId | string | ✓ | 하트비트를 보내는 서버의 ID |

**Response (200 OK):**
```json
{
  "message": "Heartbeat received"
}
```

**Error Responses:**
- `404 Not Found` — 등록되지 않은 서버 ID
- `500 Internal Server Error` — 처리 오류

**Example:**
```bash
# 10초마다 하트비트 전송
curl -X PUT http://localhost:5002/v1/internal/server/heartbeat \
  -H "Content-Type: application/json" \
  -d '{"serverId":"game-server-01"}'
```

---

## 2. Player Session Management

### 2.1 플레이어 입장
**POST** `/player/enter`

**Description:** 플레이어가 특정 게임 서버에 입장하였음을 등록합니다. 플레이어 세션을 추적하는 데 사용됩니다.

**Request:**
```json
{
  "serverId": "game-server-01",
  "mapId": "map-dungeon-1",
  "accountUid": 12345
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| serverId | string | ✓ | 플레이어가 입장하는 게임 서버 ID |
| mapId | string | ✓ | 플레이어가 입장하는 맵 ID |
| accountUid | long | ✓ | 플레이어의 계정 UID (Login Server에서 발급) |

**Response (200 OK):**
```json
{
  "message": "Player entered successfully"
}
```

**Error Responses:**
- `400 Bad Request` — 필수 파라미터 누락 또는 invalid
- `500 Internal Server Error` — 입장 처리 실패

**Example:**
```bash
curl -X POST http://localhost:5002/v1/internal/player/enter \
  -H "Content-Type: application/json" \
  -d '{
    "serverId": "game-server-01",
    "mapId": "map-dungeon-1",
    "accountUid": 12345
  }'
```

---

### 2.2 플레이어 퇴장
**POST** `/player/leave`

**Description:** 플레이어가 게임 서버에서 퇴장하였음을 등록합니다. 플레이어 세션을 정리하는 데 사용됩니다.

**Request:**
```json
{
  "serverId": "game-server-01",
  "mapId": "map-dungeon-1",
  "accountUid": 12345
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| serverId | string | ✓ | 플레이어가 퇴장하는 게임 서버 ID |
| mapId | string | ✓ | 플레이어가 퇴장하는 맵 ID |
| accountUid | long | ✓ | 플레이어의 계정 UID |

**Response (200 OK):**
```json
{
  "message": "Player left successfully"
}
```

**Error Responses:**
- `400 Bad Request` — 필수 파라미터 누락 또는 invalid
- `500 Internal Server Error` — 퇴장 처리 실패

**Example:**
```bash
curl -X POST http://localhost:5002/v1/internal/player/leave \
  -H "Content-Type: application/json" \
  -d '{
    "serverId": "game-server-01",
    "mapId": "map-dungeon-1",
    "accountUid": 12345
  }'
```

---

## 3. Server Discovery

### 3.1 사용 가능한 서버 찾기
**POST** `/find-server`

**Description:** 클라이언트가 접속할 수 있는 게임 서버를 찾습니다. 월드와 맵을 기준으로 적절한 서버를 반환합니다.

**주요 로직:**
1. 특정 월드의 모든 활성 서버 조회
2. 선택사항: 특정 맵 필터링
3. 여유 있는 서버 선택 (최대 플레이어 미도달)
4. 호스트와 포트 반환

**Request:**
```json
{
  "worldId": "world-1",
  "mapId": "map-town-1"
}
```

**Parameters:**
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| worldId | string | ✓ | 접속할 월드 ID |
| mapId | string | ✗ | 특정 맵을 원하는 경우 지정 (선택사항) |

**Response (200 OK):**
```json
{
  "host": "192.168.1.100",
  "port": 9001,
  "serverId": "game-server-01"
}
```

**Response (404 Not Found):**
```json
{
  "message": "No available server found"
}
```

**가능한 시나리오:**
- 모든 서버가 가득 찬 경우 (currentPlayers >= maxPlayers)
- 해당 월드에 등록된 서버가 없는 경우
- 서버가 오프라인 상태인 경우

**Error Responses:**
- `400 Bad Request` — 필수 파라미터 누락
- `500 Internal Server Error` — 서버 탐색 오류

**Example:**
```bash
# 특정 월드의 사용 가능한 서버 찾기
curl -X POST http://localhost:5002/v1/internal/find-server \
  -H "Content-Type: application/json" \
  -d '{
    "worldId": "world-1"
  }'

# 특정 월드와 맵의 서버 찾기
curl -X POST http://localhost:5002/v1/internal/find-server \
  -H "Content-Type: application/json" \
  -d '{
    "worldId": "world-1",
    "mapId": "map-dungeon-1"
  }'
```

---

## Response Models

### GameServerInfo
등록된 게임 서버 정보:

```json
{
  "serverId": "game-server-01",
  "worldId": "world-1",
  "mapId": "map-dungeon-1",
  "host": "192.168.1.100",
  "port": 9001,
  "maxPlayers": 100,
  "currentPlayers": 45,
  "status": "Active",
  "lastHeartbeat": "2024-03-07T13:30:00Z"
}
```

**Fields:**
| Field | Type | Description |
|-------|------|-------------|
| serverId | string | 서버 고유 ID |
| worldId | string | 소속 월드 ID |
| mapId | string | 맵 ID (선택사항) |
| host | string | 접속 호스트 |
| port | int | 접속 포트 |
| maxPlayers | int | 최대 플레이어 수 |
| currentPlayers | int | 현재 접속 중인 플레이어 수 |
| status | enum | 서버 상태 (Active, Inactive, Full) |
| lastHeartbeat | string | 마지막 하트비트 시간 (ISO 8601) |

### ServerStatus Enum
```
Active   — 정상 운영 중
Inactive — 오프라인 (하트비트 없음)
Full     — 최대 플레이어 도달
```

---

## Server Architecture

### Player Session Flow

```
1. [Game Server] 플레이어 입장
   POST /player/enter
   ↓
2. [World Server] 플레이어 세션 등록
   - serverId, mapId, accountUid 저장
   - currentPlayers 증가
   ↓
3. [Game Server] 플레이어 퇴장
   POST /player/leave
   ↓
4. [World Server] 플레이어 세션 정리
   - currentPlayers 감소
```

### Server Registration Flow

```
1. [Game Server] 시작
   POST /server/register
   ↓
2. [World Server] 서버 정보 저장
   - serverId, worldId, host, port, maxPlayers
   - status = Active
   ↓
3. [Game Server] 주기적 하트비트
   PUT /server/heartbeat (10-30초마다)
   ↓
4. [World Server] 활성 상태 유지
   - lastHeartbeat 업데이트
   - 일정 시간 하트비트 없으면 status = Inactive
```

---

## Monitoring & Health Checks

**서버 모니터링 지표:**
- **Heartbeat Timeout:** 30초 이상 하트비트 없으면 Inactive 표시
- **Player Capacity:** currentPlayers >= maxPlayers이면 Full 표시
- **Server Response Time:** 일반적으로 < 100ms 예상

**권장 하트비트 주기:** 10-20초

---

## Usage Examples

### Game Server 시작 흐름

```bash
# 1. 서버 등록
curl -X POST http://localhost:5002/v1/internal/server/register \
  -H "Content-Type: application/json" \
  -d '{
    "serverId": "game-server-01",
    "worldId": "world-1",
    "mapId": "map-dungeon-1",
    "host": "192.168.1.100",
    "port": 9001,
    "maxPlayers": 100
  }'

# 2. 주기적 하트비트 (10초마다)
while true; do
  sleep 10
  curl -X PUT http://localhost:5002/v1/internal/server/heartbeat \
    -H "Content-Type: application/json" \
    -d '{"serverId":"game-server-01"}'
done
```

### 플레이어 로그인 후 서버 접속

```bash
# 1. Login Server에서 인증 후 accessToken 획득

# 2. World Server에서 게임 서버 찾기
SERVER=$(curl -X POST http://localhost:5002/v1/internal/find-server \
  -H "Content-Type: application/json" \
  -d '{"worldId":"world-1"}')
# 응답: { "host": "192.168.1.100", "port": 9001, "serverId": "game-server-01" }

# 3. 게임 서버(TCP)에 접속하여 플레이 시작

# 4. 플레이어 입장 등록
curl -X POST http://localhost:5002/v1/internal/player/enter \
  -H "Content-Type: application/json" \
  -d '{
    "serverId": "game-server-01",
    "mapId": "map-dungeon-1",
    "accountUid": 12345
  }'

# 5. 플레이어 퇴장 등록
curl -X POST http://localhost:5002/v1/internal/player/leave \
  -H "Content-Type: application/json" \
  -d '{
    "serverId": "game-server-01",
    "mapId": "map-dungeon-1",
    "accountUid": 12345
  }'
```

---

## Error Handling

| HTTP Status | Error | Description |
|-------------|-------|-------------|
| 200 | N/A | 성공 |
| 400 | Bad Request | 필수 파라미터 누락 또는 invalid |
| 404 | Not Found | 서버 미등록 또는 플레이어 없음 |
| 500 | Internal Server Error | 서버 오류 |

---

## Related Documentation

- [Login Server API](./LOGIN_API.md) — 플레이어 인증
- [Ranking Server API](./RANKING_API.md) — 플레이어 순위 관리
