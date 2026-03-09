using CSharp.World.Models;
using CSharp.World.Services;

namespace CSharp.World.Endpoints;

public static class PlayerEndpoints
{
    public static void MapPlayerEndpoints(this IEndpointRouteBuilder app)
    {
        var group = app.MapGroup("/v1/internal/player").WithTags("Player");

        group.MapPost("/enter", PlayerEnter);
        group.MapPost("/leave", PlayerLeave);
    }

    private static async Task<IResult> PlayerEnter(
        PlayerEnterRequest request,
        ServerRegistryService registryService,
        ILogger<ServerRegistryService> logger)
    {
        try
        {
            var result = await registryService.PlayerEnterAsync(
                request.ServerId,
                request.MapId,
                request.AccountUid);
            
            if (!result)
            {
                return Results.BadRequest("Failed to process player enter");
            }

            return Results.Ok(new { message = "Player entered successfully" });
        }
        catch (Exception ex)
        {
            logger.LogError(ex, "Error processing player enter for accountUid: {AccountUid}", request.AccountUid);
            return Results.Problem("Failed to process player enter");
        }
    }

    private static async Task<IResult> PlayerLeave(
        PlayerLeaveRequest request,
        ServerRegistryService registryService,
        ILogger<ServerRegistryService> logger)
    {
        try
        {
            var result = await registryService.PlayerLeaveAsync(
                request.ServerId,
                request.MapId,
                request.AccountUid);
            
            if (!result)
            {
                return Results.BadRequest("Failed to process player leave");
            }

            return Results.Ok(new { message = "Player left successfully" });
        }
        catch (Exception ex)
        {
            logger.LogError(ex, "Error processing player leave for accountUid: {AccountUid}", request.AccountUid);
            return Results.Problem("Failed to process player leave");
        }
    }
}
