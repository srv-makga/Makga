# Patch Server API

**Base URL:** `http://localhost:5012`  
**Port:** 5012  
**인증 방식:** 공개 (버전 조회) / 내부 네트워크 전용 (버전 관리)

---

## 개요

패치 서버는 클라이언트 버전 관리, 강제 업데이트 여부 확인, 패치 파일 목록 제공을 담당한다.

---

## 헬스 체크

| 메서드 | 경로 | 설명 |
|---|---|---|
| `GET` | `/health` | 서버 상태 확인 |

---

## 공개 엔드포인트

### GET `/v1/version/{platform}` — 최신 버전 정보 조회

**Path Parameters:** `platform` — `android` / `ios` / `pc`

**Response 200 OK**
```json
{
  "platform": "android",
  "latest_version": "1.5.0",
  "minimum_version": "1.3.0",
  "force_update": false,
  "patch_notes": "버그 수정 및 성능 개선",
  "store_url": "https://play.google.com/..."
}
```

`force_update`가 `true`이면 클라이언트는 반드시 업데이트해야 한다.

---

### GET `/v1/version/{version}/files` — 패치 파일 목록 조회

**Path Parameters:** `version` (string) — 패치 버전

**Response 200 OK**
```json
{
  "version": "1.5.0",
  "files": [
    {
      "path": "assets/textures/hero.png",
      "size": 204800,
      "hash": "sha256:abc123...",
      "cdn_url": "https://cdn.makga.com/patch/1.5.0/assets/textures/hero.png"
    }
  ]
}
```

---

## 내부 엔드포인트 (내부 네트워크 전용)

### POST `/v1/internal/version` — 버전 등록/수정

**Request Body**
```json
{
  "platform": "android",
  "version": "1.5.0",
  "minimum_version": "1.3.0",
  "force_update": false,
  "patch_notes": "버그 수정 및 성능 개선",
  "store_url": "https://play.google.com/..."
}
```

**Validation:** `platform`과 `version`은 필수.

**Response 200 OK**
```json
{ "message": "버전 정보 저장 완료" }
```

---

### GET `/v1/internal/versions` — 전체 버전 목록 조회

**Response 200 OK** — 플랫폼별 전체 버전 이력

---

## 관련 문서

- [아키텍처 구조도](../architecture/PATCH_ARCHITECTURE.md)
