using Login.Endpoints;
using Login.Repository;
using Login.Services;
using WebCommon;

var builder = WebApplication.CreateBuilder(args);

// 공통 미들웨어 등록: Serilog 파일 로깅, OpenAPI(Swagger), 전역 예외 핸들러
builder.AddWebCommon();

// MySQL: account_db 연결 풀 (appsettings.json의 MySQL:ConnectionString)
builder.AddMySql();

// Redis: Refresh Token 저장소 (appsettings.json의 Redis:ConnectionString)
builder.AddRedis();

// JWT 서명 키·만료 설정은 프로세스 전반에서 공유 → Singleton
builder.Services.AddSingleton<JwtService>();

// Refresh Token(Redis) 관리 서비스 → Singleton (IConnectionMultiplexer 재사용)
builder.Services.AddSingleton<RefreshTokenService>();

// ─────────────────────────────────────────────────────────────────────
// 외부 플랫폼 OAuth 검증 서비스
//   · GoogleAuthService — Google ID Token 서명 검증 (Google.Apis.Auth 라이브러리)
//   · AppleAuthService  — Apple ID Token 서명 검증 (JWKS 캐시 포함)
// HttpClient 인스턴스를 내부에서 직접 관리하므로 Singleton으로 등록
// ─────────────────────────────────────────────────────────────────────
builder.Services.AddSingleton<GoogleAuthService>();
builder.Services.AddSingleton<AppleAuthService>();

// AccountRepository·AccountService: DB 연결은 요청 단위로 열고 닫음 → Scoped
builder.Services.AddScoped<AccountRepository>();
builder.Services.AddScoped<AccountService>();

var app = builder.Build();

// 시작 시 연결 문자열·포트 등 주요 설정을 로그로 출력
app.LogStartupConfiguration();

// /v1/auth 하위 모든 엔드포인트 등록
EndpointMapper.MapAll(app);

app.Run();
