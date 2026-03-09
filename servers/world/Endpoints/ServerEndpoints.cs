using CSharp.World.Models;
using CSharp.World.Services;

namespace CSharp.World.Endpoints;

public static class ServerEndpoints
{
    public static void MapServerEndpoints(this IEndpointRouteBuilder app)
    {
        var group = app.MapGroup("/v1/internal/server").WithTags("Server");

        group.MapPost("/register", RegisterServer);
        group.MapPut("/heartbeat", Heartbeat);
    }

    private static async Task<IResult> RegisterServer(
        RegisterServerRequest request,
        ServerRegistryService registryService,
        ILogger<ServerRegistryService> logger)
    {
        try
        {
            var result = await registryService.RegisterServerAsync(request);
            
            if (!result)
            {
                return Results.Problem("Failed to register server");
            }

            return Results.Ok(new { message = "Server registered successfully" });
        }
        catch (Exception ex)
        {
            logger.LogError(ex, "Error registering server: {ServerId}", request.ServerId);
            return Results.Problem("Failed to register server");
        }
    }

    private static async Task<IResult> Heartbeat(
        HeartbeatRequest request,
        ServerRegistryService registryService,
        ILogger<ServerRegistryService> logger)
    {
        try
        {
            var result = await registryService.HeartbeatAsync(request.ServerId);
            
            if (!result)
            {
                return Results.NotFound();
            }

            return Results.Ok(new { message = "Heartbeat received" });
        }
        catch (Exception ex)
        {
            logger.LogError(ex, "Error processing heartbeat for server: {ServerId}", request.ServerId);
            return Results.Problem("Failed to process heartbeat");
        }
    }
}
