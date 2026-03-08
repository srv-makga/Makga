using CSharp.Queue.Models;
using CSharp.Queue.Repository;
using CSharp.Queue.Services.ServerResolver;

namespace CSharp.Queue.Services;

public class QueueProcessorService : BackgroundService
{
    private readonly IServiceProvider _serviceProvider;
    private readonly ILogger<QueueProcessorService> _logger;
    private readonly IConfiguration _configuration;

    private int ProcessorIntervalMs => _configuration.GetValue("Queue:ProcessorIntervalMs", 500);
    private TimeSpan ReadyTtl => TimeSpan.FromSeconds(_configuration.GetValue("Queue:ReadyTtlSeconds", 60));

    public QueueProcessorService(
        IServiceProvider serviceProvider,
        ILogger<QueueProcessorService> logger,
        IConfiguration configuration)
    {
        _serviceProvider = serviceProvider;
        _logger = logger;
        _configuration = configuration;
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        _logger.LogInformation("QueueProcessorService started");

        while (!stoppingToken.IsCancellationRequested)
        {
            try
            {
                await ProcessQueuesAsync();
                await CleanupExpiredTicketsAsync();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error in QueueProcessorService");
            }

            await Task.Delay(ProcessorIntervalMs, stoppingToken);
        }

        _logger.LogInformation("QueueProcessorService stopped");
    }

    private async Task ProcessQueuesAsync()
    {
        using var scope = _serviceProvider.CreateScope();
        var repository = scope.ServiceProvider.GetRequiredService<QueueRepository>();
        var serverResolver = scope.ServiceProvider.GetRequiredService<IWorldServerResolver>();
        var sseService = scope.ServiceProvider.GetRequiredService<SseNotificationService>();

        var worldIds = await repository.GetAllWorldIdsAsync();

        foreach (var worldId in worldIds)
        {
            try
            {
                var capacity = await repository.GetCapacityAsync(worldId);
                var online = await repository.GetOnlineAsync(worldId);
                var availableSlots = Math.Max(0, capacity - online);

                if (availableSlots == 0)
                {
                    continue;
                }

                var entries = await repository.PopFromQueueAsync(worldId, availableSlots);

                foreach (var entry in entries)
                {
                    var ticketId = entry.Element.ToString();
                    var ticket = await repository.GetTicketAsync(ticketId);

                    if (ticket == null || ticket.Status != QueueStatus.Waiting)
                    {
                        continue;
                    }

                    var serverInfo = await serverResolver.ResolveAsync(worldId);
                    if (serverInfo == null)
                    {
                        _logger.LogWarning("Failed to resolve server for worldId: {WorldId}, re-queuing ticket", worldId);
                        await repository.AddToQueueAsync(worldId, ticketId, entry.Score);
                        continue;
                    }

                    var entryToken = Guid.NewGuid().ToString("N");
                    serverInfo.EntryToken = entryToken;

                    await repository.SetEntryTokenAsync(entryToken, new EntryTokenInfo
                    {
                        AccountUid = ticket.AccountUid,
                        WorldId = worldId,
                        ServerId = serverInfo.ServerId ?? ""
                    }, ReadyTtl);

                    await repository.UpdateTicketStatusAsync(
                        ticketId,
                        QueueStatus.Ready,
                        DateTime.UtcNow,
                        serverInfo,
                        ReadyTtl);

                    await sseService.NotifyReadyAsync(ticketId, serverInfo);

                    _logger.LogInformation("Ticket {TicketId} for user {AccountUid} promoted to READY for world {WorldId}",
                        ticketId, ticket.AccountUid, worldId);
                }

                await UpdateWaitingPositionsAsync(worldId, repository, sseService);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error processing queue for worldId: {WorldId}", worldId);
            }
        }
    }

    private async Task UpdateWaitingPositionsAsync(string worldId, QueueRepository repository, SseNotificationService sseService)
    {
        try
        {
            var queueLength = await repository.GetQueueLengthAsync(worldId);
            if (queueLength == 0)
            {
                return;
            }
        }
        catch (Exception ex)
        {
            _logger.LogWarning(ex, "Failed to update waiting positions for worldId: {WorldId}", worldId);
        }
    }

    private async Task CleanupExpiredTicketsAsync()
    {
        using var scope = _serviceProvider.CreateScope();
        var repository = scope.ServiceProvider.GetRequiredService<QueueRepository>();

        var worldIds = await repository.GetAllWorldIdsAsync();

        foreach (var worldId in worldIds)
        {
            try
            {
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error cleaning up expired tickets for worldId: {WorldId}", worldId);
            }
        }
    }
}
