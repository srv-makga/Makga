namespace AdminTool.Models;

public class DashboardViewModel
{
    public bool DbOk { get; set; }
    public bool RedisOk { get; set; }
    public List<ServerStatusInfo> ServerStatuses { get; set; } = new();
    public Dictionary<string, long> TableCounts { get; set; } = new();
    public string? RedisInfo { get; set; }
}

public class ServerStatusInfo
{
    public string Name { get; set; } = "";
    public string Host { get; set; } = "";
    public int Port { get; set; }
    public bool IsOnline { get; set; }
    public long? PingMs { get; set; }
}

public class TableViewModel
{
    public string TableName { get; set; } = "";
    public List<ColumnMeta> Columns { get; set; } = new();
    public List<Dictionary<string, object?>> Rows { get; set; } = new();
    public int Total { get; set; }
    public int Page { get; set; } = 1;
    public int PageSize { get; set; } = 50;
    public string? Search { get; set; }
    public List<string> AllTables { get; set; } = new();
    public int TotalPages => (int)Math.Ceiling((double)Total / PageSize);
}

public class CreateAccountViewModel
{
    public string Username { get; set; } = "";
    public string Password { get; set; } = "";
    public string ConfirmPassword { get; set; } = "";
    public string Role { get; set; } = "viewer";
}

public class EditAccountViewModel
{
    public int Id { get; set; }
    public string Username { get; set; } = "";
    public string Role { get; set; } = "viewer";
    public bool IsActive { get; set; } = true;
    public string? NewPassword { get; set; }
    public string? ConfirmPassword { get; set; }
}

public class PlayerSearchViewModel
{
    public string? Query { get; set; }
    public string? TableName { get; set; }
    public List<string> PlayerTables { get; set; } = new();
    public List<Dictionary<string, object?>> Results { get; set; } = new();
    public List<ColumnMeta> Columns { get; set; } = new();
    public int Total { get; set; }
}

public class PlayerDetailViewModel
{
    public string PlayerId { get; set; } = "";
    public Dictionary<string, TableData> Tables { get; set; } = new();
}

public class TableData
{
    public List<ColumnMeta> Columns { get; set; } = new();
    public List<Dictionary<string, object?>> Rows { get; set; } = new();
}

public class PacketSendRequest
{
    public string ServerName { get; set; } = "";
    public ushort PacketId { get; set; }
    public byte PacketKey { get; set; } = 0;
    public string? PayloadHex { get; set; }
}

public class PacketSendResponse
{
    public bool Success { get; set; }
    public string? Error { get; set; }
    public ushort ResponsePacketId { get; set; }
    public byte ResponseKey { get; set; }
    public string? ResponseHex { get; set; }
    public long ElapsedMs { get; set; }
}

public class RedisKeyInfo
{
    public string Key { get; set; } = "";
    public string Type { get; set; } = "";
    public long Ttl { get; set; } = -1;
    public string? StringValue { get; set; }
    public Dictionary<string, string>? HashValue { get; set; }
    public List<string>? ListValue { get; set; }
    public List<string>? SetValue { get; set; }
    public List<ZSetEntry>? ZSetValue { get; set; }
}

public class ZSetEntry
{
    public string Member { get; set; } = "";
    public double Score { get; set; }
}

public class ApiCallRequest
{
    public string Url { get; set; } = "";
    public string Method { get; set; } = "GET";
    public string? HeadersJson { get; set; }
    public string? Body { get; set; }
    public string? AuthType { get; set; }
    public string? AuthValue { get; set; }
}

public class ApiCallResponse
{
    public bool Success { get; set; }
    public int StatusCode { get; set; }
    public string? StatusText { get; set; }
    public string? ResponseHeaders { get; set; }
    public string? Body { get; set; }
    public long ElapsedMs { get; set; }
    public string? Error { get; set; }
}
