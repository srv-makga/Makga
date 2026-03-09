using CSharp.World.Models;

namespace CSharp.World.Services;

public class HeartbeatMonitorService : BackgroundService
{
    private readonly IServiceProvider _serviceProvider;
    private readonly ILogger<HeartbeatMonitorService> _logger;
    private readonly IConfiguration _configuration;

    private TimeSpan HeartbeatTimeout => TimeSpan.FromSeconds(_configuration.GetValue("World:HeartbeatTimeoutSeconds", 30));
    private TimeSpan CheckInterval => TimeSpan.FromSeconds(10);

    public HeartbeatMonitorService(
        IServiceProvider serviceProvider,
        ILogger<HeartbeatMonitorService> logger,
        IConfiguration configuration)
    {
        _serviceProvider = serviceProvider;
        _logger = logger;
        _configuration = configuration;
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        _logger.LogInformation("HeartbeatMonitorService started");

        while (!stoppingToken.IsCancellationRequested)
        {
            try
            {
                await CheckDeadServersAsync();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error in HeartbeatMonitorService");
            }

            await Task.Delay(CheckInterval, stoppingToken);
        }

        _logger.LogInformation("HeartbeatMonitorService stopped");
    }

    private async Task CheckDeadServersAsync()
    {
        using var scope = _serviceProvider.CreateScope();
        var registryService = scope.ServiceProvider.GetRequiredService<ServerRegistryService>();

        var now = DateTime.UtcNow;
        var deadServers = new List<string>();

        foreach (var server in registryService.GetAllServers())
        {
            if (now - server.LastHeartbeat > HeartbeatTimeout)
            {
                deadServers.Add(server.ServerId);
                _logger.LogWarning("Server {ServerId} heartbeat timeout. Last heartbeat: {LastHeartbeat}",
                    server.ServerId, server.LastHeartbeat);
            }
        }

        foreach (var serverId in deadServers)
        {
            await registryService.RemoveServerAsync(serverId);
        }
    }
}
