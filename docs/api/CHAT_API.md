# Chat Server API

**Base URL:** `http://localhost:5002`  
**Port:** 5002  
**프로토콜:** HTTP/1.1 + WebSocket  
**인증 방식:** JWT Bearer Token

---

## 개요

채팅 서버는 WebSocket 기반의 실시간 채팅과 HTTP 기반의 채팅 이력 조회를 담당한다.  
채널 유형: `world`(전체) / `zone`(구역) / `party`(파티) / `guild`(길드) / `whisper`(귓속말)

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## WebSocket 연결

### GET `/ws` — WebSocket 채팅 연결

**Query Parameters**

| 파라미터 | 타입 | 필수 | 설명 |
|---|---|---|---|
| `token` | string | ✓ | JWT Access Token |

**연결 성공 시 서버 → 클라이언트:**
```json
{ "type": "connected", "account_uid": 1001, "account_name": "홍길동" }
```

**클라이언트 → 서버 메시지 형식:**
```json
{ "channel": "world", "content": "안녕하세요!", "target_uid": null }
```

| 필드 | 타입 | 설명 |
|---|---|---|
| `channel` | string | `world` / `zone` / `party` / `guild` / `whisper` |
| `content` | string | 메시지 내용 (최대 500자) |
| `target_uid` | long? | 귓속말 대상 UID (`whisper` 전용) |

**서버 → 클라이언트 브로드캐스트:**
```json
{
  "type": "chat",
  "channel": "world",
  "account_uid": 1001,
  "account_name": "홍길동",
  "content": "안녕하세요!",
  "timestamp": "2026-03-29T12:00:00Z"
}
```

**오류 코드**

| 코드 | 설명 |
|---|---|
| 401 | JWT 토큰 누락 또는 검증 실패 |
| 403 | 채팅 금지(Mute) 상태 |
| 429 | 전송 속도 초과 (Rate Limit) |

---

## HTTP API

### GET `/v1/chat/history` — 채팅 이력 조회

**Headers:** `Authorization: Bearer {token}`

**Query Parameters**

| 파라미터 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `channel` | string | `world` | 채널 유형 |
| `limit` | int | 50 | 조회 개수 (최대 100) |
| `before` | string? | - | 커서 기반 페이징 (이전 메시지 ID) |

**Response 200 OK**
```json
{
  "messages": [
    {
      "id": "msg_001",
      "channel": "world",
      "account_uid": 1001,
      "account_name": "홍길동",
      "content": "안녕하세요!",
      "timestamp": "2026-03-29T12:00:00Z"
    }
  ],
  "next_cursor": "msg_000"
}
```

---

## 채널 유형 설명

| 채널 | 범위 | 설명 |
|---|---|---|
| `world` | 서버 전체 | 모든 플레이어에게 전달 |
| `zone` | 현재 구역 | 같은 맵/구역 플레이어에게 전달 |
| `party` | 파티원 | 파티 구성원에게만 전달 |
| `guild` | 길드원 | 길드 구성원에게만 전달 |
| `whisper` | 1:1 | `target_uid` 지정 필수 |

---

## 관련 문서

- [아키텍처 구조도](../architecture/CHAT_ARCHITECTURE.md)
- [Gateway API](GATEWAY_API.md)
