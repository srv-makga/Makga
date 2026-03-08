namespace CSharp.Queue.Models;

public class QueueTicket
{
    public string TicketId { get; set; } = string.Empty;
    public long AccountUid { get; set; }
    public string WorldId { get; set; } = string.Empty;
    public QueueStatus Status { get; set; } = QueueStatus.Waiting;
    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
    public DateTime? ReadyAt { get; set; }
    public ServerInfo? ServerInfo { get; set; }
}

public enum QueueStatus
{
    Waiting,
    Ready,
    Entered,
    Expired
}

public class ServerInfo
{
    public string Host { get; set; } = string.Empty;
    public int Port { get; set; }
    public string? EntryToken { get; set; }
    public string? ServerId { get; set; }
}

public class JoinRequest
{
    public string WorldId { get; set; } = string.Empty;
    public long AccountUid { get; set; }
    public string SessionToken { get; set; } = string.Empty;
}

public class JoinResponse
{
    public string TicketId { get; set; } = string.Empty;
    public QueueStatus Status { get; set; }
    public int Position { get; set; }
    public ServerInfo? ServerInfo { get; set; }
}

public class QueueStatusResponse
{
    public QueueStatus Status { get; set; }
    public int Position { get; set; }
    public ServerInfo? ServerInfo { get; set; }
}

public class EntryTokenInfo
{
    public long AccountUid { get; set; }
    public string WorldId { get; set; } = string.Empty;
    public string ServerId { get; set; } = string.Empty;
}
