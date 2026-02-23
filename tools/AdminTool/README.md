# Makga Admin Tool

ASP.NET Core 8 기반 웹 어드민 툴.

## 실행 방법

```bash
cd tools/admin_tool
dotnet restore
dotnet run
# → http://localhost:5000
```

## 최초 설정

1. MySQL에 `setup.sql` 실행
   ```sql
   SOURCE tools/admin_tool/setup.sql;
   ```
2. `appsettings.json`에서 DB / Redis / 서버 정보 수정
3. 기본 계정: `admin` / `admin1234` (로그인 후 반드시 변경)

## 주요 기능

| 페이지 | 설명 |
|--------|------|
| **대시보드** | DB·Redis·서버 상태 + 게임 데이터 통계 |
| **어드민 계정** | 계정 생성·수정·삭제 (superadmin 전용) |
| **플레이어** | user/account/character 테이블 검색·조회·아이템/재화 지급·밴 |
| **게임 데이터** | 모든 DB 테이블 조회·수정·추가·삭제 |
| **서버 관리** | 각 서버 핑 + FlatBuffer 패킷 전송 |
| **Redis** | SCAN 키 브라우저, 값 조회·수정·삭제 |
| **API 호출** | HTTP 요청 GUI 클라이언트 |

## 권한 구조

| 역할 | 권한 |
|------|------|
| `viewer` | 읽기 전용 |
| `admin` | DB·Redis·패킷 전송 편집 가능 |
| `superadmin` | 전체 권한 (계정 관리, FLUSHDB, 행 삭제 등) |

## 패킷 형식

`servers/common/net_packet.h` 기준:
```
[packet_id: uint16 LE][packet_size: uint32 LE][packet_key: uint8]  ← 7바이트 헤더
[FlatBuffer payload ...]
```
서버 관리 페이지에서 패킷 ID, Key, 페이로드(Hex)를 입력해 전송합니다.
