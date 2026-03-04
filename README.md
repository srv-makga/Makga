<div align="center">

# Makga

**고성능 멀티플레이어 게임 서버 백엔드 프레임워크**

[![C++](https://img.shields.io/badge/C%2B%2B-20-blue?logo=cplusplus)](https://en.cppreference.com/)
[![C#](https://img.shields.io/badge/C%23-.NET%208-purple?logo=dotnet)](https://dotnet.microsoft.com/)
[![Go](https://img.shields.io/badge/Go-1.21-00ADD8?logo=go)](https://golang.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows)](https://www.microsoft.com/windows)

[개요](#-개요) • [아키텍처](#-아키텍처) • [빌드](#-빌드) • [시작하기](#-시작하기) • [문서](#-문서)

</div>

---

## 📖 개요

Makga는 MMORPG 수준의 동시접속자를 처리하도록 설계된 게임 서버 백엔드 프레임워크입니다. Windows IOCP / RIO 기반 네트워크 레이어, Unreal Engine 스타일 ECS 게임 서버, Go 매치메이킹 서버, C# 랭킹 API까지 하나의 모노레포에서 관리합니다.

### ✨ 주요 특징

- **듀얼 네트워크 I/O** — IOCP(범용)와 RIO(저지연) 선택 가능, 동일한 추상 인터페이스
- **ECS 게임 서버** — SoA(Structure of Arrays) 메모리 레이아웃, CPU 코어 단위 TickGroup 병렬 처리
- **Lock-free 자료구조** — MPSC Queue, Ring Buffer, Lock-free Queue 등 고성능 컨테이너 내장
- **FlatBuffers 프로토콜** — 역직렬화 zero-copy, 스키마 기반 패킷 관리
- **멀티 언어 서버군** — C++(게임/커뮤니티), C#(랭킹), Go(매치메이킹)
- **운영 인프라 포함** — Docker, Kubernetes, Terraform, Admin Web Tool 일체 제공

---

## 🏗️ 아키텍처

```
┌─────────────────────────────────────────────────────────────────┐
│                          클라이언트                              │
└──────────────────────────────┬──────────────────────────────────┘
                               │ FlatBuffers over TCP/UDP
          ┌────────────────────┼────────────────────┐
          ▼                    ▼                    ▼
  ┌───────────────┐   ┌───────────────┐   ┌───────────────┐
  │  Game Server  │   │Match Server   │   │Ranking Server │
  │   (C++ ECS)   │   │     (Go)      │   │   (C# / ASP)  │
  └───────┬───────┘   └───────────────┘   └───────┬───────┘
          │                                        │
  ┌───────▼───────┐                       ┌───────▼───────┐
  │Community Server│                      │     Redis     │
  │  (C++ / 소셜) │                       │  (랭킹 스토어) │
  └───────────────┘                       └───────────────┘
          │
  ┌───────▼───────┐
  │     MySQL     │
  │  (게임 데이터) │
  └───────────────┘
```

### 서버 구성

| 서버 | 언어 | 경로 | 역할 |
|------|------|------|------|
| Game Server | C++20 (MSVC) | `servers/game/` | ECS 기반 게임 월드, 전투, AI, 물리 |
| Community Server | C++20 | `servers/community/` | 길드, 파티, 친구 |
| Ranking Server | C# (.NET 8) | `servers/ranking/` | Redis 기반 실시간 랭킹 API |
| Match Server | Go | `servers/match/` | 매치메이킹 풀 관리 |
| Admin Tool | C# (ASP.NET 8) | `tools/AdminTool/` | 운영 웹 콘솔 |
| DeployTool Manager | C# (Blazor Server) | `tools/DeployTool/Manager/` | 서버 배포 대시보드 |
| DeployTool Agent | C# (.NET Service) | `tools/DeployTool/Agent/` | 서버 사이드 배포 에이전트 |

---

## 📦 핵심 라이브러리 (`libs/cpp/`)

### Network (`libs/cpp/network/`)

두 가지 I/O 구현이 동일한 인터페이스(`net_service`, `net_session`)로 제공됩니다.

| 구현 | 파일 | 특징 |
|------|------|------|
| **IOCP** | `iocp_core`, `iocp_service`, `iocp_acceptor`, `iocp_connector`, `iocp_session` | 범용, 높은 안정성 |
| **RIO** | `rio_core`, `rio_service`, `rio_acceptor`, `rio_connector`, `rio_session` | 저지연, 커널 복사 최소화 |
| **UDP** | `udp_socket`, `udp_peer`, `udp_group`, `udp_control_service` | 실시간 위치 동기화 |

```
[IOCP] AcceptEx/ConnectEx → IOCP → GetQueuedCompletionStatus → Dispatch
[RIO]  RIOReceive/RIOSend → RIO CQ → IOCP 알림 → RIODequeueCompletion → Dispatch
```

### Algorithm Library (`libs/cpp/libraries/include/`)

C++20 Named Module(`.ixx`) 형태로 제공되는 유틸리티 모음.

| 모듈 | 설명 |
|------|------|
| `mpsc_queue.ixx` | Multi-Producer Single-Consumer Lock-free 큐 |
| `lock_free_queue.ixx` | Lock-free FIFO 큐 |
| `ring_buffer.ixx` | 고정 크기 원형 버퍼 |
| `double_queue.ixx` | Swap 기반 이중 버퍼 큐 |
| `object_pool.ixx` / `thread_local_pool.ixx` | 객체 풀 (전역 / 스레드 로컬) |
| `thread_pool.ixx` / `thread_group.ixx` | 스레드 풀 및 그룹 관리 |
| `brain_tree.ixx` | 행동 트리 (AI Behavior Tree) |
| `message_broker.ixx` | Pub/Sub 메시지 브로커 |
| `observer.ixx` | 이벤트 옵저버 |
| `ecs.ixx` / `archtype.ixx` / `component.ixx` / `entity.ixx` | ECS 기본 타입 |
| `redis_connector.ixx` | hiredis 래퍼 (Redis 클라이언트) |
| `binary_writer.ixx` | 바이너리 직렬화 |
| `ahocorasick.ixx` | 아호-코라식 문자열 검색 |
| `vector3.ixx` / `math_*.ixx` / `geometry.ixx` | 3D 수학 라이브러리 |

### 3rd-party

`hiredis`, `flatbuffers`, `recastnavigation`, `asio`, `rapidjson`, `glog`

---

## 🎮 게임 서버 ECS 구조 (`servers/game/`)

Unreal Engine 스타일의 ECS(Entity Component System)를 구현합니다.

```
GameWorld (Singleton)
├── ComponentManager        ← SoA(Structure of Arrays) 데이터 저장소
│   ├── TransformComponent[]    위치, 회전, 스케일
│   ├── PhysicsComponent[]      속도, 질량, 충돌
│   ├── StatComponent[]         HP, MP, 레벨
│   ├── AIComponent[]           타겟, 상태, 행동 트리
│   ├── CombatComponent[]       공격력, 방어력, 스킬
│   └── TerrainComponent[]      지형 그리드 참조
│
├── World Systems           ← 순수 로직 (데이터 없음)
│   ├── PhysicsSystem       중력, 힘, 충돌 검사
│   ├── MovementSystem      위치 갱신, 지형 그리드
│   ├── CombatSystem        쿨다운, 데미지 계산
│   └── AISystem            타겟 탐색, 상태 머신
│
├── TickGroupManager        ← CPU 코어 기반 병렬 처리
│   └── TickGroup[0..N]     각 그룹 = 독립 스레드
│
└── NewActor                ← 데이터 컨테이너
    ├── ActorId
    └── MPSC MessageQueue   비동기 명령 수신
```

**프레임당 시스템 실행 순서:** `Physics → Movement → Combat → AI → TickGroup`

---

## 🛠️ 빌드

### 요구 사항

- Windows 10/11 (x64)
- Visual Studio 2022 (MSVC v143, C++20)
- .NET 8 SDK
- Go 1.21+

### 서버 솔루션 빌드

```powershell
# Debug
msbuild .\servers\servers.sln /p:Configuration=Debug /p:Platform=x64

# Release
msbuild .\servers\servers.sln /p:Configuration=Release /p:Platform=x64
```

### 단위 테스트 빌드

```powershell
msbuild .\unit_test\unit_test.vcxproj /p:Configuration=Debug /p:Platform=x64
# 결과물: unit_test\x64\Debug\unit_test.dll
# 실행: Visual Studio Test Explorer
```

### FlatBuffers 스키마 컴파일

```powershell
scripts\flatbuffer\compile.bat
# 출력: scripts\flatbuffer\out\cpp\
```

### Ranking Server (C#)

```powershell
cd servers\ranking
dotnet restore
dotnet run
```

### DeployTool (C#)

```powershell
# Agent — 각 게임 서버에서 실행
cd tools\DeployTool\Agent
dotnet restore
dotnet run   # 기본 포트: 7700

# Manager — 운영자 PC에서 실행
cd tools\DeployTool\Manager
dotnet restore
dotnet run   # → http://localhost:5000
```

---

## 🚀 시작하기

### 1. 데이터베이스 초기화

```sql
-- tables/sql/ 내 SQL 파일들을 MySQL에 적용
SOURCE tables/sql/item.sql;
SOURCE tables/sql/monster.sql;
-- ... (전체 테이블 목록은 tables/sql/ 디렉토리 참조)
```

### 2. 설정 파일 수정

환경별 설정 파일(`config/dev/`, `config/staging/`, `config/production/`)을 편집합니다.

### 3. Admin Tool 실행

```powershell
cd tools\AdminTool
# setup.sql을 MySQL에 먼저 실행
dotnet restore
dotnet run
# → http://localhost:5000
# 기본 계정: admin / admin1234 (최초 로그인 후 변경 필수)
```

### 4. DeployTool 설정

```powershell
# 1) 각 게임 서버에서 Agent 실행 (appsettings.json에서 WorkDir, PatchDirs 설정)
cd tools\DeployTool\Agent && dotnet run

# 2) Manager의 serverlist.json에 에이전트 등록
# tools/DeployTool/Manager/serverlist.json
# [{ "Name": "game-01", "Host": "10.0.0.1", "Port": 7700, "Group": "game", "Tags": ["prod"] }]

# 3) Manager 실행
cd tools\DeployTool\Manager && dotnet run  # → http://localhost:5000
```

### 5. 패킷 형식

```
┌──────────────┬──────────────┬────────────┬──────────────────────┐
│ packet_id    │ packet_size  │ packet_key │ FlatBuffer payload   │
│  (uint16 LE) │  (uint32 LE) │  (uint8)   │       ...            │
└──────────────┴──────────────┴────────────┴──────────────────────┘
             ◄────── 7 bytes header ──────►
```

---

## 🧪 테스트

`unit_test/` 디렉토리에 핵심 모듈별 단위 테스트가 포함됩니다.

| 테스트 파일 | 대상 모듈 |
|------------|----------|
| `mpsc_queue_test.cpp` | MPSC Lock-free 큐 |
| `ringbuffer_test.cpp` | Ring Buffer |
| `doublequeue_test.cpp` | Double Queue |
| `object_pool_test.cpp` | Object Pool |
| `binary_writer_test.cpp` | Binary Writer |
| `math_collision_test.cpp` | 충돌 수학 |
| `math_geometry_test.cpp` | 기하 수학 |
| `math_motion_test.cpp` | 물리 운동 |
| `math_probability_test.cpp` | 확률 수학 |
| `vector3_test.cpp` | 벡터 연산 |
| `singleton_test.cpp` | 싱글톤 |
| `algorithm_test.cpp` | 알고리즘 |
| `datetime_test.cpp` | 날짜/시간 |
| `random_test.cpp` | 난수 생성 |
| `db_test.cpp` | DB 연결 |
| `netbuffer_test.cpp` | 네트워크 버퍼 |
| `udp_peer_test.cpp` / `udp_group_test.cpp` | UDP |

---

## 🗄️ 데이터 테이블 (`tables/`)

게임 데이터를 SQL, CSV, JSON, Excel 형식으로 관리합니다.

<details>
<summary>SQL 테이블 목록 (클릭하여 펼치기)</summary>

| 테이블 | 설명 |
|--------|------|
| `item`, `item_equip`, `item_consume`, `item_effect` | 아이템 시스템 |
| `monster`, `monster_stat`, `monster_skill`, `monster_drop` | 몬스터 |
| `skill`, `skill_level`, `skill_effect`, `skill_condition` | 스킬 |
| `quest`, `quest_condition`, `quest_reward` | 퀘스트 |
| `craft_recipe`, `craft_material` | 제작 |
| `dungeon`, `spawn`, `map`, `map_object` | 월드 |
| `gacha`, `gacha_item` | 가챠 |
| `shop`, `shop_item` | 상점 |
| `buff`, `effect` | 버프/효과 |
| `season`, `ranking_reward` | 시즌/랭킹 |
| `achievement`, `title` | 업적/칭호 |
| `character_class`, `level_exp`, `stat_growth` | 캐릭터 성장 |

</details>

---

## 📁 디렉토리 구조

```
Makga/
├── servers/                  서버 소스
│   ├── game/                 게임 서버 (C++ ECS)
│   ├── community/            커뮤니티 서버 (C++)
│   ├── ranking/              랭킹 서버 (C#)
│   ├── match/                매치 서버 (Go)
│   ├── common/               서버 공통 코드 (C++)
│   └── servers.sln           Visual Studio 솔루션
├── libs/cpp/                 C++ 핵심 라이브러리
│   ├── libraries/include/    알고리즘·유틸 모듈 (.ixx)
│   ├── network/include/      네트워크 I/O 모듈 (.ixx)
│   └── 3rdparty/             외부 의존성
├── tools/                    운영 도구
│   ├── AdminTool/            웹 어드민 (C# ASP.NET)
│   ├── DeployTool/           서버 배포 도구
│   │   ├── Manager/          배포 대시보드 (Blazor Server)
│   │   ├── Agent/            배포 에이전트 (.NET Service)
│   │   └── Common/           공유 패킷 프로토콜
│   └── TableExporter/        데이터 테이블 익스포터
├── tables/                   게임 데이터
│   ├── sql/                  MySQL DDL
│   ├── csv/                  CSV 데이터
│   ├── json/                 JSON 데이터
│   └── excel/                Excel 원본
├── scripts/
│   └── flatbuffer/           FlatBuffers 스키마 및 컴파일 스크립트
├── config/
│   ├── dev/                  개발 환경 설정
│   ├── staging/              스테이징 환경 설정
│   └── production/           운영 환경 설정
├── infra/
│   ├── docker/               Dockerfile
│   ├── k8s/                  Kubernetes 매니페스트
│   ├── terraform/            IaC (클라우드 리소스)
│   └── ci_cd/                CI/CD 파이프라인
├── unit_test/                단위 테스트
└── docs/                     문서
```

---

## 📋 프로토콜 스키마 (`scripts/flatbuffer/`)

| 파일 | 설명 |
|------|------|
| `enum.fbs` | 패킷 ID 열거형 (`PacketId`) |
| `result.fbs` | 결과 코드 (`ResultCode`) |
| `version.fbs` | 버전 정보 |

---

## 🚢 DeployTool (`tools/DeployTool/`)

파일 패치 배포, 프로세스 모니터링, 로그 조회, 서비스 제어를 단일 웹 대시보드에서 관리하는 운영 도구입니다.

```
Manager (Blazor Server)                     Agent (.NET 호스티드 서비스)
────────────────────────────                ────────────────────────────
AgentRegistry                               AgentServer (TcpListener)
  └─ AgentClient × N ──── TCP/JSON ────►    AgentSession
       ├─ HeartbeatLoop                       └─ CommandDispatcher
       ├─ SendAsync<T>                             ├─ FileService
       └─ PutFileChunkedAsync                      ├─ ProcessService
                                                   ├─ ResourceService
Pages                                              └─ ShellService / ...
  ├─ Dashboard   에이전트 목록 + 리소스 현황
  ├─ Patch       파일 업로드 → 선택 에이전트에 배포
  ├─ AgentDetail 파일 탐색기, 로그, 프로세스
  └─ ProcessMonitor  감시 프로세스 CRUD
```

**패치 배포 흐름:**
1. Manager가 파일을 **4MB 청크**로 분할 → `UploadChunk` 패킷 순차 전송
2. Agent가 `{바이너리경로}/Download/{파일명}.tmp` 에 청크 조립
3. 마지막 청크 수신 시 **SHA-256 검증** → 성공하면 `PatchDirs` (설정된 배포 경로)에 복사
4. ZIP 파일은 자동으로 압축 해제 후 복사

**DeployTool 패킷 프로토콜** — 7바이트 헤더(게임 서버와 동일 포맷) + **JSON 페이로드**:
```
[packet_id: uint16 LE][payload_len: uint32 LE][key: uint8] | JSON bytes...
```

---

## 🔧 Admin Tool 권한

| 역할 | 권한 |
|------|------|
| `viewer` | 읽기 전용 |
| `admin` | DB·Redis·패킷 전송 편집 |
| `superadmin` | 전체 권한 (계정 관리, FLUSHDB, 행 삭제) |

---

## 🤝 기여

1. 이 저장소를 포크합니다.
2. 기능 브랜치를 생성합니다. (`git checkout -b feature/AmazingFeature`)
3. 변경사항을 커밋합니다. (`git commit -m 'Add AmazingFeature'`)
4. 브랜치에 푸시합니다. (`git push origin feature/AmazingFeature`)
5. Pull Request를 생성합니다.

---

## 📄 라이선스

이 프로젝트는 MIT 라이선스를 따릅니다. 자세한 내용은 [LICENSE](LICENSE) 파일을 참조하세요.

---

<div align="center">
Made with ❤️ for high-performance game servers
</div>
