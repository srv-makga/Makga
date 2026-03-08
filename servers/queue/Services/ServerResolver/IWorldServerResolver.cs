using CSharp.Queue.Models;

namespace CSharp.Queue.Services.ServerResolver;

public interface IWorldServerResolver
{
    Task<ServerInfo?> ResolveAsync(string worldId);
}
