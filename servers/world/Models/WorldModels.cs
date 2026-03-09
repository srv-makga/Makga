namespace CSharp.World.Models;

public class GameServerInfo
{
    public string ServerId { get; set; } = string.Empty;
    public string WorldId { get; set; } = string.Empty;
    public string? MapId { get; set; }
    public string Host { get; set; } = string.Empty;
    public int Port { get; set; }
    public int MaxPlayers { get; set; }
    public int CurrentPlayers { get; set; }
    public ServerStatus Status { get; set; } = ServerStatus.Active;
    public DateTime LastHeartbeat { get; set; } = DateTime.UtcNow;
}

public enum ServerStatus
{
    Active,
    Inactive,
    Full
}

public class RegisterServerRequest
{
    public string ServerId { get; set; } = string.Empty;
    public string WorldId { get; set; } = string.Empty;
    public string? MapId { get; set; }
    public string Host { get; set; } = string.Empty;
    public int Port { get; set; }
    public int MaxPlayers { get; set; }
}

public class HeartbeatRequest
{
    public string ServerId { get; set; } = string.Empty;
}

public class PlayerEnterRequest
{
    public string ServerId { get; set; } = string.Empty;
    public string MapId { get; set; } = string.Empty;
    public long AccountUid { get; set; }
}

public class PlayerLeaveRequest
{
    public string ServerId { get; set; } = string.Empty;
    public string MapId { get; set; } = string.Empty;
    public long AccountUid { get; set; }
}

public class FindServerRequest
{
    public string WorldId { get; set; } = string.Empty;
    public string? MapId { get; set; }
}

public class FindServerResponse
{
    public string Host { get; set; } = string.Empty;
    public int Port { get; set; }
    public string ServerId { get; set; } = string.Empty;
}
