using CSharp.Queue.Models;
using System.Text.Json;

namespace CSharp.Queue.Services.ServerResolver;

public class RemoteServerResolver : IWorldServerResolver
{
    private readonly HttpClient _httpClient;
    private readonly string _worldServerUrl;
    private readonly ILogger<RemoteServerResolver> _logger;

    public RemoteServerResolver(HttpClient httpClient, IConfiguration configuration, ILogger<RemoteServerResolver> logger)
    {
        _httpClient = httpClient;
        _worldServerUrl = configuration["Queue:ServerResolver:RemoteUrl"] 
            ?? throw new InvalidOperationException("Queue:ServerResolver:RemoteUrl not configured");
        _logger = logger;
    }

    public async Task<ServerInfo?> ResolveAsync(string worldId)
    {
        try
        {
            var request = new { worldId };
            var response = await _httpClient.PostAsJsonAsync(_worldServerUrl, request);
            
            if (!response.IsSuccessStatusCode)
            {
                _logger.LogWarning("World Server returned {StatusCode} for worldId: {WorldId}", 
                    response.StatusCode, worldId);
                return null;
            }

            var serverInfo = await response.Content.ReadFromJsonAsync<ServerInfo>();
            return serverInfo;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to resolve server from World Server for worldId: {WorldId}", worldId);
            return null;
        }
    }
}
