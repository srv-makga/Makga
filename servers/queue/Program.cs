using StackExchange.Redis;
using CSharp.Queue.Repository;
using CSharp.Queue.Services;
using CSharp.Queue.Services.ServerResolver;
using CSharp.Queue.Endpoints;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

var redisConnectionString = builder.Configuration.GetConnectionString("Redis") 
    ?? throw new InvalidOperationException("Redis connection string not configured");
var redis = await ConnectionMultiplexer.ConnectAsync(redisConnectionString);
builder.Services.AddSingleton<IConnectionMultiplexer>(redis);

builder.Services.AddSingleton<QueueRepository>();
builder.Services.AddSingleton<ISessionValidationService, SessionValidationService>();

var resolverType = builder.Configuration["Queue:ServerResolver:Type"]?.ToLower() ?? "remote";
if (resolverType == "direct")
{
    builder.Services.AddSingleton<IWorldServerResolver, DirectServerResolver>();
}
else
{
    builder.Services.AddHttpClient<IWorldServerResolver, RemoteServerResolver>();
}

builder.Services.AddSingleton<QueueService>();
builder.Services.AddSingleton<SseNotificationService>();

builder.Services.AddHostedService<QueueProcessorService>();

var app = builder.Build();

if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.MapQueueEndpoints();

app.Run();
