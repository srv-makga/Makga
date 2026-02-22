using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Serilog;
using Serilog.Events;

namespace WebCommon;

public static class WebCommonExtensions
{
    /// <summary>
    /// Configures Serilog (console + rolling file) and Output Cache from appsettings.
    /// Call this before builder.Build().
    /// </summary>
    public static WebApplicationBuilder AddWebCommon(this WebApplicationBuilder builder)
    {
        var logPath = builder.Configuration["Logging:File:Path"] ?? "logs/server-.log";

        Log.Logger = new LoggerConfiguration()
            .MinimumLevel.Information()
            .MinimumLevel.Override("Microsoft", LogEventLevel.Warning)
            .MinimumLevel.Override("Microsoft.Hosting.Lifetime", LogEventLevel.Information)
            .WriteTo.Console(outputTemplate: "[{Timestamp:HH:mm:ss} {Level:u3}] {Message:lj}{NewLine}{Exception}")
            .WriteTo.File(
                logPath,
                rollingInterval: RollingInterval.Day,
                outputTemplate: "[{Timestamp:yyyy-MM-dd HH:mm:ss} {Level:u3}] {Message:lj}{NewLine}{Exception}")
            .CreateLogger();

        builder.Host.UseSerilog();
        builder.Services.AddOutputCache();
        return builder;
    }

    /// <summary>
    /// Activates Output Cache middleware and logs bound URLs and key configuration values at startup.
    /// Call this after builder.Build().
    /// </summary>
    public static WebApplication LogStartupConfiguration(this WebApplication app)
    {
        app.UseOutputCache();

        var cfg = app.Configuration;
        var log = app.Logger;

        log.LogInformation("========== Server Configuration ==========");

        var httpUrl  = cfg["Kestrel:Endpoints:Http:Url"];
        var httpsUrl = cfg["Kestrel:Endpoints:Https:Url"];
        var httpProto  = cfg["Kestrel:Endpoints:Http:Protocols"]  ?? "default";
        var httpsProto = cfg["Kestrel:Endpoints:Https:Protocols"] ?? "default";
        if (httpUrl  is not null) log.LogInformation("HTTP  : {Url} [{Protocols}]", httpUrl,  httpProto);
        if (httpsUrl is not null) log.LogInformation("HTTPS : {Url} [{Protocols}]", httpsUrl, httpsProto);

        var redis = cfg["Redis:ConnectionString"];
        if (redis is not null) log.LogInformation("Redis : {Connection}", redis);

        var cacheTtl = cfg["RankingCache:TopExpirationSeconds"];
        if (cacheTtl is not null) log.LogInformation("Cache : top rankings expire in {Seconds}s", cacheTtl);

        var logPath = cfg["Logging:File:Path"] ?? "logs/server-.log";
        log.LogInformation("LogFile : {Path}", logPath);

        log.LogInformation("==========================================");
        return app;
    }
}
