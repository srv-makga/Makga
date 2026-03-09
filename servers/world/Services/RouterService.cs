using CSharp.World.Models;

namespace CSharp.World.Services;

public class RouterService
{
    private readonly ServerRegistryService _registryService;
    private readonly ILogger<RouterService> _logger;
    private readonly IConfiguration _configuration;
    private int _roundRobinIndex = 0;

    private string RoutingPolicy => _configuration["World:RoutingPolicy"]?.ToLower() ?? "least-loaded";

    public RouterService(ServerRegistryService registryService, ILogger<RouterService> logger, IConfiguration configuration)
    {
        _registryService = registryService;
        _logger = logger;
        _configuration = configuration;
    }

    public FindServerResponse? FindServer(FindServerRequest request)
    {
        var candidates = _registryService.GetServersByWorld(request.WorldId)
            .Where(s => s.Status == ServerStatus.Active)
            .Where(s => s.CurrentPlayers < s.MaxPlayers)
            .ToList();

        if (!candidates.Any())
        {
            _logger.LogWarning("No available servers for world: {WorldId}", request.WorldId);
            return null;
        }

        if (!string.IsNullOrEmpty(request.MapId))
        {
            var mapServers = candidates.Where(s => s.MapId == request.MapId).ToList();
            if (mapServers.Any())
            {
                candidates = mapServers;
            }
        }

        GameServerInfo? selected = RoutingPolicy switch
        {
            "least-loaded" => SelectLeastLoaded(candidates),
            "map-affinity" => SelectMapAffinity(candidates, request.MapId),
            "round-robin" => SelectRoundRobin(candidates),
            _ => SelectLeastLoaded(candidates)
        };

        if (selected == null)
        {
            return null;
        }

        _logger.LogInformation("Selected server {ServerId} for world {WorldId} using policy {Policy}",
            selected.ServerId, request.WorldId, RoutingPolicy);

        return new FindServerResponse
        {
            Host = selected.Host,
            Port = selected.Port,
            ServerId = selected.ServerId
        };
    }

    private GameServerInfo? SelectLeastLoaded(List<GameServerInfo> candidates)
    {
        return candidates.OrderBy(s => s.CurrentPlayers).FirstOrDefault();
    }

    private GameServerInfo? SelectMapAffinity(List<GameServerInfo> candidates, string? mapId)
    {
        if (!string.IsNullOrEmpty(mapId))
        {
            var mapServer = candidates.FirstOrDefault(s => s.MapId == mapId);
            if (mapServer != null)
                return mapServer;
        }

        return SelectLeastLoaded(candidates);
    }

    private GameServerInfo? SelectRoundRobin(List<GameServerInfo> candidates)
    {
        if (!candidates.Any())
            return null;

        var index = Interlocked.Increment(ref _roundRobinIndex) % candidates.Count;
        return candidates[index];
    }
}
