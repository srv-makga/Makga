using CSharp.Queue.Models;
using System.Collections.Concurrent;
using System.Text.Json;

namespace CSharp.Queue.Services;

public class SseNotificationService
{
    private readonly ConcurrentDictionary<string, List<StreamWriter>> _connections = new();
    private readonly ILogger<SseNotificationService> _logger;

    public SseNotificationService(ILogger<SseNotificationService> logger)
    {
        _logger = logger;
    }

    public void RegisterConnection(string ticketId, StreamWriter writer)
    {
        _connections.AddOrUpdate(
            ticketId,
            _ => new List<StreamWriter> { writer },
            (_, list) =>
            {
                list.Add(writer);
                return list;
            });

        _logger.LogInformation("SSE connection registered for ticket: {TicketId}", ticketId);
    }

    public void UnregisterConnection(string ticketId, StreamWriter writer)
    {
        if (_connections.TryGetValue(ticketId, out var list))
        {
            list.Remove(writer);
            if (list.Count == 0)
            {
                _connections.TryRemove(ticketId, out _);
            }
        }

        _logger.LogInformation("SSE connection unregistered for ticket: {TicketId}", ticketId);
    }

    public async Task NotifyAsync(string ticketId, object eventData)
    {
        if (!_connections.TryGetValue(ticketId, out var writers))
        {
            return;
        }

        var json = JsonSerializer.Serialize(eventData);
        var message = $"data: {json}\n\n";

        var deadConnections = new List<StreamWriter>();

        foreach (var writer in writers.ToList())
        {
            try
            {
                await writer.WriteAsync(message);
                await writer.FlushAsync();
            }
            catch (Exception ex)
            {
                _logger.LogWarning(ex, "Failed to send SSE notification to ticket: {TicketId}", ticketId);
                deadConnections.Add(writer);
            }
        }

        foreach (var dead in deadConnections)
        {
            UnregisterConnection(ticketId, dead);
        }
    }

    public async Task NotifyPositionUpdateAsync(string ticketId, int position)
    {
        await NotifyAsync(ticketId, new
        {
            status = "waiting",
            position
        });
    }

    public async Task NotifyReadyAsync(string ticketId, ServerInfo serverInfo)
    {
        await NotifyAsync(ticketId, new
        {
            status = "ready",
            position = 0,
            serverInfo
        });
    }
}
