# API Gateway — API 명세서

> **언어:** Go 1.22 | **위치:** `infra/gateway/` | **포트:** 8080 (HTTP), 9090 (Metrics)
>
> ⚠️ **마이그레이션 노트:** 기존 C# Gateway(`servers/gateway/`)는 Go 구현으로 대체되었습니다. 진입점 포트가 `5000 → 8080`으로 변경되었습니다.

API Gateway는 Makga 백엔드의 단일 진입점(Single Entry Point)으로, 모든 클라이언트 요청을 수신하여 적절한 업스트림 서버로 라우팅합니다. 고루틴 기반 비동기 처리로 대량의 동시 요청을 처리하며, JWT 인증, 레이트 리밋, 서킷 브레이커, 로드 밸런싱 기능을 내장합니다.

---

## 아키텍처 개요

```
클라이언트
    │
    ▼
[Nginx L4/L7 로드밸런서]  ← Active-Active (3 인스턴스, HA 구성)
    │
    ▼
[Go Gateway :8080]
    ├── JWT 검증 미들웨어
    ├── 레이트 리밋 (Redis Sliding Window)
    ├── 서킷 브레이커 (per-cluster)
    ├── 라우터 (경로 매칭)
    └── 리버스 프록시 (고루틴 풀)
         ├── login-cluster      → :5001
         ├── world-cluster      → :5004
         ├── ranking-cluster    → :5005
         ├── queue-cluster      → :5002
         ├── auction-cluster    → :5006
         ├── chat-cluster       → :5007  (WebSocket 지원)
         ├── billing-cluster    → :5010
         ├── mail-cluster       → :5008
         ├── social-cluster     → :5011
         ├── gm-cluster         → :5012
         ├── analytics-cluster  → :5013
         ├── divine-cluster     → :5014
         ├── season-cluster     → :5015
         ├── housing-cluster    → :5016
         ├── scheduler-cluster  → :5009
         ├── event-cluster      → :5017
         ├── notification-cluster → :5018
         ├── report-cluster     → :5019
         ├── anticheat-cluster  → :5020
         ├── raidlobby-cluster  → :5021
         ├── guildwar-cluster   → :5022
         ├── crossserver-cluster → :5023
         ├── recommendation-cluster → :5024
         └── replay-cluster     → :5025
```

---

## 라우팅 테이블

| 외부 경로 | 업스트림 클러스터 | 인증 필요 | 설명 |
|---|---|---|---|
| `/v1/auth/*` | `login-cluster` | 아니오 | 로그인, 회원가입, 토큰 갱신 |
| `/v1/world/*` | `world-cluster` | 예 | 서버 목록, 레지스트리 |
| `/v1/ranking/*` | `ranking-cluster` | 예 | 랭킹 조회 |
| `/v1/queue/*` | `queue-cluster` | 예 | 대기열 입장/상태 |
| `/v1/auction/*` | `auction-cluster` | 예 | 경매장 |
| `/v1/chat/*` | `chat-cluster` | 예 | 채팅 (WebSocket 포함) |
| `/v1/billing/*` | `billing-cluster` | 예 | 결제, IAP 검증 |
| `/v1/mail/*` | `mail-cluster` | 예 | 우편함 |
| `/v1/social/*` | `social-cluster` | 예 | 친구, 길드 |
| `/v1/gm/*` | `gm-cluster` | 예 (GM 역할) | GM 관리 도구 |
| `/v1/analytics/*` | `analytics-cluster` | 예 (내부) | 분석, AppsFlyer |
| `/v1/divine/*` | `divine-cluster` | 예 | 신 시스템 |
| `/v1/season/*` | `season-cluster` | 예 | 시즌 패스 |
| `/v1/housing/*` | `housing-cluster` | 예 | 하우징 |
| `/v1/event/*` | `event-cluster` | 예 | 이벤트/가챠 |
| `/v1/notification/*` | `notification-cluster` | 예 | 푸시 알림 |
| `/v1/report/*` | `report-cluster` | 예 | 신고 |
| `/v1/anticheat/*` | `anticheat-cluster` | 예 (내부) | 안티치트 |
| `/v1/raid/*` | `raidlobby-cluster` | 예 | 레이드 로비 |
| `/v1/guild-war/*` | `guildwar-cluster` | 예 | 길드전 |
| `/v1/crossserver/*` | `crossserver-cluster` | 예 | 크로스서버 |
| `/v1/recommendation/*` | `recommendation-cluster` | 예 | 추천 |
| `/v1/replay/*` | `replay-cluster` | 예 | 리플레이 |

---

## 헬스체크 엔드포인트

### `GET /live`

**Liveness Probe** — 프로세스 생존 여부 확인 (Kubernetes kubelet용).

**Response 200 OK:**
```json
{
  "status": "ok",
  "timestamp": "2026-05-17T10:00:00Z"
}
```

---

### `GET /ready`

**Readiness Probe** — 요청 처리 준비 완료 여부 확인. Redis 연결 및 업스트림 클러스터 상태를 포함합니다.

**Response 200 OK:**
```json
{
  "status": "ready",
  "upstreams": {
    "login-cluster": "healthy",
    "world-cluster": "healthy",
    "ranking-cluster": "healthy"
  },
  "redis": "connected",
  "timestamp": "2026-05-17T10:00:00Z"
}
```

**Response 503 Service Unavailable:**
```json
{
  "status": "not_ready",
  "upstreams": {
    "login-cluster": "unhealthy",
    "world-cluster": "healthy"
  },
  "redis": "connected"
}
```

---

### `GET /health`

**상세 헬스 정보** — 모든 클러스터의 상태, 서킷 브레이커 상태, 고루틴 수를 반환합니다.

**Response 200 OK:**
```json
{
  "status": "healthy",
  "instance_id": "gateway-1",
  "uptime_seconds": 86400,
  "goroutines": 142,
  "clusters": {
    "login-cluster": {
      "status": "healthy",
      "endpoints": [
        { "url": "http://login:8080", "healthy": true, "latency_ms": 12 }
      ],
      "circuit_breaker": "closed",
      "requests_total": 150234,
      "errors_total": 12
    }
  },
  "redis": {
    "status": "connected",
    "latency_ms": 1
  }
}
```

---

### `GET /health/services`

**모든 업스트림 서비스 상태 요약** — 기존 C# Gateway 호환 형식.

**Response 200 OK:**
```json
{
  "gateway": "healthy",
  "services": {
    "login": "healthy",
    "ranking": "healthy",
    "chat": "healthy",
    "billing": "healthy",
    "world": "healthy"
  }
}
```

---

### `GET /metrics`

**Prometheus 메트릭** — 포트 9090에서 노출. Prometheus 텍스트 형식으로 반환합니다.

**주요 메트릭:**

| 메트릭 이름 | 타입 | 설명 |
|---|---|---|
| `gateway_requests_total` | Counter | 총 요청 수 (cluster, method, status 레이블) |
| `gateway_request_duration_seconds` | Histogram | 요청 처리 시간 분포 |
| `gateway_upstream_errors_total` | Counter | 업스트림 오류 수 |
| `gateway_circuit_breaker_state` | Gauge | 서킷 브레이커 상태 (0=closed, 1=open, 2=half-open) |
| `gateway_rate_limit_rejected_total` | Counter | 레이트 리밋으로 거부된 요청 수 |
| `gateway_active_connections` | Gauge | 현재 활성 연결 수 |
| `gateway_goroutines` | Gauge | 현재 고루틴 수 |

---

## 인증 방식

### JWT Bearer 토큰

인증이 필요한 모든 엔드포인트는 `Authorization` 헤더에 Bearer 토큰을 포함해야 합니다.

```http
Authorization: Bearer <JWT_TOKEN>
```

**JWT Payload 구조:**
```json
{
  "sub": "user_id_12345",
  "role": "player",
  "server_id": "server_01",
  "iat": 1716000000,
  "exp": 1716086400
}
```

**내부 서비스로 전달되는 헤더 (기존 C# Gateway 호환):**
```http
X-Account-Uid: 1001
X-Account-Name: 홍길동
X-Account-Role: player
```

**지원 역할 (role):**

| 역할 | 접근 가능 경로 |
|---|---|
| `player` | 일반 게임 API 전체 |
| `gm` | `/v1/gm/*` 포함 전체 |
| `internal` | `/v1/analytics/*`, `/v1/anticheat/*` 포함 전체 |

---

## Rate Limiting

Redis Sliding Window 알고리즘 기반. 클라이언트 IP 또는 JWT `sub`(사용자 ID) 기준으로 제한합니다.

| 경로 | 제한 | 윈도우 |
|---|---|---|
| `/v1/auth/self/login` | 10 req | 1분 |
| `/v1/auth/self/register` | 5 req | 1분 |
| `/v1/auth/token/refresh` | 30 req | 1분 |
| 일반 API | 300 req | 1분 |
| GM API | 1000 req | 1분 |
| WebSocket (`/ws`) | 연결 수 제한 없음 | - |

**레이트 리밋 초과 응답 (429 Too Many Requests):**
```json
{
  "error": "rate_limit_exceeded",
  "retry_after": 30,
  "message": "Too many requests. Please try again later."
}
```

**응답 헤더:**
```http
X-RateLimit-Limit: 300
X-RateLimit-Remaining: 0
X-RateLimit-Reset: 1716000060
Retry-After: 30
```

---

## 서킷 브레이커

클러스터별 독립 서킷 브레이커. 업스트림 장애 시 빠른 실패(Fail-Fast)로 전체 시스템 보호.

| 상태 | 조건 | 동작 |
|---|---|---|
| **Closed** (정상) | 오류율 < 50% | 모든 요청 통과 |
| **Open** (차단) | 오류율 ≥ 50% (10초 내 5회 이상 실패) | 즉시 503 반환 |
| **Half-Open** (복구 시도) | Open 상태 30초 경과 | 요청 1개 통과하여 성공 시 Closed로 전환 |

**서킷 브레이커 Open 응답 (503 Service Unavailable):**
```json
{
  "error": "service_unavailable",
  "cluster": "login-cluster",
  "message": "Service temporarily unavailable. Circuit breaker is open."
}
```

---

## 로드 밸런싱

클러스터별 로드 밸런싱 알고리즘 설정 가능.

| 알고리즘 | 설정값 | 적합한 서버 |
|---|---|---|
| Round-Robin | `round_robin` | 무상태 API 서버 |
| Least-Connection | `least_conn` | 처리 시간이 긴 서버 (채팅, 경매장) |

---

## WebSocket 지원

`/v1/chat/*` 경로는 WebSocket 업그레이드를 지원합니다.

```http
GET /v1/chat/ws HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Authorization: Bearer <JWT_TOKEN>
```

WebSocket 연결은 JWT 검증 후 `chat-cluster`로 프록시됩니다. 연결이 유지되는 동안 레이트 리밋은 적용되지 않습니다.

---

## 오류 응답 형식

모든 오류는 동일한 JSON 형식으로 반환됩니다.

```json
{
  "error": "error_code",
  "message": "사람이 읽을 수 있는 오류 메시지",
  "request_id": "req_abc123def456",
  "timestamp": "2026-05-17T10:00:00Z"
}
```

| HTTP 상태 | 오류 코드 | 설명 |
|---|---|---|
| 400 | `bad_request` | 잘못된 요청 형식 |
| 401 | `unauthorized` | JWT 토큰 없음 또는 만료 |
| 403 | `forbidden` | 권한 부족 |
| 404 | `not_found` | 라우팅 규칙 없음 |
| 429 | `rate_limit_exceeded` | 레이트 리밋 초과 |
| 502 | `bad_gateway` | 업스트림 서버 오류 |
| 503 | `service_unavailable` | 서킷 브레이커 Open 또는 업스트림 다운 |
| 504 | `gateway_timeout` | 업스트림 응답 시간 초과 (기본 30초) |

---

## 요청 추적

모든 요청에 고유 ID가 부여됩니다.

**요청 헤더 (클라이언트 → Gateway):**
```http
X-Request-ID: client-provided-id   # 선택적
```

**응답 헤더 (Gateway → 클라이언트):**
```http
X-Request-ID: req_abc123def456     # 없으면 자동 생성
X-Gateway-Instance: gateway-1
X-Response-Time: 15ms
```

---

## 설정 파일 위치

| 파일 | 설명 |
|---|---|
| `infra/gateway/config/gateway.yaml` | 전체 라우팅/클러스터/레이트리밋/서킷브레이커 설정 |
| `infra/docker/gateway-ha.yml` | 고가용성 Docker Compose (3 인스턴스 + Nginx + Redis Sentinel) |
| `infra/docker/configs/nginx-gateway.conf` | Nginx 로드밸런서 설정 |
| `infra/docker/configs/redis-sentinel.conf` | Redis Sentinel HA 설정 |
| `infra/k8s/gateway/deployment.yaml` | Kubernetes Deployment + HPA + PDB |

---

## 관련 문서

- [아키텍처 구조도](../architecture/GATEWAY_ARCHITECTURE.md)
- [Login API](LOGIN_API.md)
