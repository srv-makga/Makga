# Makga API Documentation

이 폴더는 Makga MMORPG 서버 프레임워크의 전체 API 명세서를 포함한다.

---

## 서버 목록

| 서버 | 파일 | 포트 | 설명 |
|---|---|---|---|
| API Gateway | [GATEWAY_API.md](GATEWAY_API.md) | 5000 | 단일 진입점, 라우팅/인증 |
| Login | [LOGIN_API.md](LOGIN_API.md) | 5001 | 인증, OAuth, 2FA, 토큰 관리 |
| Chat | [CHAT_API.md](CHAT_API.md) | 5002 | WebSocket 실시간 채팅 |
| Queue | [QUEUE_API.md](QUEUE_API.md) | 5003 | 게임 서버 접속 대기열 |
| Auction | [AUCTION_API.md](AUCTION_API.md) | 5004 | 경매장 |
| Billing | [BILLING_API.md](BILLING_API.md) | 5005 | 인앱 결제, 지갑, 구독 |
| Ranking | [RANKING_API.md](RANKING_API.md) | 5006 | 랭킹 조회 |
| World | [WORLD_API.md](WORLD_API.md) | 5007 | 서버 레지스트리, 라우팅 |
| GM | [GM_API.md](GM_API.md) | 5008 | 계정 관리, 밴, 지급 |
| Social | [SOCIAL_API.md](SOCIAL_API.md) | 5009 | 친구, 차단 |
| Mail | [MAIL_API.md](MAIL_API.md) | 5010 | 우편함, 아이템 지급 |
| Notification | [NOTIFICATION_API.md](NOTIFICATION_API.md) | 5011 | 푸시 알림 |
| Patch | [PATCH_API.md](PATCH_API.md) | 5012 | 클라이언트 버전 관리 |
| Report | [REPORT_API.md](REPORT_API.md) | 5013 | 신고, 밴 관리 |
| Housing | [HOUSING_API.md](HOUSING_API.md) | 5014 | 하우징, 가구 배치 |
| Event | [EVENT_API.md](EVENT_API.md) | 5015 | 이벤트, 가챠, 강화 |
| Season | [SEASON_API.md](SEASON_API.md) | 5016 | 시즌 패스 |
| Raid Lobby | [RAID_LOBBY_API.md](RAID_LOBBY_API.md) | 5017 | 레이드 파티/매칭 |
| Guild War | [GUILD_WAR_API.md](GUILD_WAR_API.md) | 5018 | 길드전 |
| Scheduler | [SCHEDULER_API.md](SCHEDULER_API.md) | 5019 | 주기적 이벤트 실행 |
| Anti-Cheat | [ANTICHEAT_API.md](ANTICHEAT_API.md) | 5020 | 이상 행동 탐지 |
| Analytics | [ANALYTICS_API.md](ANALYTICS_API.md) | 5021 | 게임 이벤트 수집/분석 |
| Recommendation | [RECOMMENDATION_API.md](RECOMMENDATION_API.md) | 5022 | 아이템/퀘스트 추천 |
| Cross-Server | [CROSSSERVER_API.md](CROSSSERVER_API.md) | 5023 | 서버 간 동기화 |
| Divine | [DIVINE_API.md](DIVINE_API.md) | 5024 | 신 시스템, 신들의 전쟁 |
| Replay | [REPLAY_API.md](REPLAY_API.md) | 5025 | 전투 기록, 리플레이 |

---

## 인증 방식

모든 공개 API는 JWT Bearer Token 인증을 사용한다.

```
Authorization: Bearer {access_token}
```

Access Token은 Login Server에서 발급받으며 15분간 유효하다.  
만료 시 Refresh Token으로 갱신한다.

---

## 관련 문서

- [아키텍처 구조도](../architecture/README.md)
- [프로젝트 분석](../PROJECT_ANALYSIS.md)
