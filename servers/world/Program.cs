using StackExchange.Redis;
using CSharp.World.Repository;
using CSharp.World.Services;
using CSharp.World.Endpoints;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

var redisConnectionString = builder.Configuration.GetConnectionString("Redis") 
    ?? throw new InvalidOperationException("Redis connection string not configured");
var redis = await ConnectionMultiplexer.ConnectAsync(redisConnectionString);
builder.Services.AddSingleton<IConnectionMultiplexer>(redis);

builder.Services.AddSingleton<WorldRepository>();
builder.Services.AddSingleton<ServerRegistryService>();
builder.Services.AddSingleton<RouterService>();

builder.Services.AddHostedService<HeartbeatMonitorService>();

var app = builder.Build();

if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.MapServerEndpoints();
app.MapPlayerEndpoints();
app.MapRouterEndpoints();

app.Run();
