using CSharp.World.Models;
using CSharp.World.Services;

namespace CSharp.World.Endpoints;

public static class RouterEndpoints
{
    public static void MapRouterEndpoints(this IEndpointRouteBuilder app)
    {
        var group = app.MapGroup("/v1/internal").WithTags("Router");

        group.MapPost("/find-server", FindServer);
    }

    private static IResult FindServer(
        FindServerRequest request,
        RouterService routerService,
        ILogger<RouterService> logger)
    {
        try
        {
            var result = routerService.FindServer(request);
            
            if (result == null)
            {
                return Results.NotFound(new { message = "No available server found" });
            }

            return Results.Ok(result);
        }
        catch (Exception ex)
        {
            logger.LogError(ex, "Error finding server for world: {WorldId}", request.WorldId);
            return Results.Problem("Failed to find server");
        }
    }
}
