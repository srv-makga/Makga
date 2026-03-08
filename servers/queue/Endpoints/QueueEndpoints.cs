using CSharp.Queue.Models;
using CSharp.Queue.Services;

namespace CSharp.Queue.Endpoints;

public static class QueueEndpoints
{
    public static void MapQueueEndpoints(this IEndpointRouteBuilder app)
    {
        var group = app.MapGroup("/v1/queue").WithTags("Queue");

        group.MapPost("/join", JoinQueue);
        group.MapGet("/status/{ticketId}", GetStatus);
        group.MapGet("/stream/{ticketId}", StreamStatus);
        group.MapDelete("/leave/{ticketId}", LeaveQueue);
    }

    private static async Task<IResult> JoinQueue(
        JoinRequest request,
        QueueService queueService,
        ILogger<QueueService> logger)
    {
        try
        {
            var response = await queueService.JoinQueueAsync(request);
            
            if (response == null)
            {
                return Results.Unauthorized();
            }

            return Results.Ok(response);
        }
        catch (Exception ex)
        {
            logger.LogError(ex, "Error joining queue");
            return Results.Problem("Failed to join queue");
        }
    }

    private static async Task<IResult> GetStatus(
        string ticketId,
        QueueService queueService,
        ILogger<QueueService> logger)
    {
        try
        {
            var status = await queueService.GetStatusAsync(ticketId);
            
            if (status == null)
            {
                return Results.NotFound();
            }

            return Results.Ok(status);
        }
        catch (Exception ex)
        {
            logger.LogError(ex, "Error getting queue status for ticket: {TicketId}", ticketId);
            return Results.Problem("Failed to get queue status");
        }
    }

    private static async Task StreamStatus(
        string ticketId,
        HttpContext context,
        QueueService queueService,
        SseNotificationService sseService,
        ILogger<QueueService> logger)
    {
        context.Response.Headers.Append("Content-Type", "text/event-stream");
        context.Response.Headers.Append("Cache-Control", "no-cache");
        context.Response.Headers.Append("Connection", "keep-alive");

        var writer = new StreamWriter(context.Response.Body);
        sseService.RegisterConnection(ticketId, writer);

        try
        {
            var status = await queueService.GetStatusAsync(ticketId);
            if (status != null)
            {
                await writer.WriteAsync($"data: {{\"status\":\"{status.Status}\",\"position\":{status.Position}}}\n\n");
                await writer.FlushAsync();
            }

            while (!context.RequestAborted.IsCancellationRequested)
            {
                await writer.WriteAsync(": keep-alive\n\n");
                await writer.FlushAsync();
                await Task.Delay(TimeSpan.FromSeconds(15), context.RequestAborted);
            }
        }
        catch (Exception ex)
        {
            logger.LogInformation("SSE connection closed for ticket: {TicketId}. Reason: {Message}", ticketId, ex.Message);
        }
        finally
        {
            sseService.UnregisterConnection(ticketId, writer);
        }
    }

    private static async Task<IResult> LeaveQueue(
        string ticketId,
        QueueService queueService,
        ILogger<QueueService> logger)
    {
        try
        {
            var result = await queueService.LeaveQueueAsync(ticketId);
            
            if (!result)
            {
                return Results.NotFound();
            }

            return Results.Ok(new { message = "Left queue successfully" });
        }
        catch (Exception ex)
        {
            logger.LogError(ex, "Error leaving queue for ticket: {TicketId}", ticketId);
            return Results.Problem("Failed to leave queue");
        }
    }
}
