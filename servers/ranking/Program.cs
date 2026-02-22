using Ranking.Endpoints;
using Ranking.Services;
using StackExchange.Redis;
using WebCommon;

var builder = WebApplication.CreateBuilder(args);
builder.AddWebCommon();

var redisConn = await ConnectionMultiplexer.ConnectAsync(
	builder.Configuration["Redis:ConnectionString"] ?? "localhost:6379");

builder.Services.AddSingleton<IConnectionMultiplexer>(redisConn);
builder.Services.AddSingleton<RankingService>();

var app = builder.Build();
app.LogStartupConfiguration();

// 서버 시작 시점에 RankingService를 즉시 인스턴스화하여 Redis 설정 오류를 조기에 감지
app.Services.GetRequiredService<RankingService>();

EndpointMapper.MapAll(app);

app.Run();
