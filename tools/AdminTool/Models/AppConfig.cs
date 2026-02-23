namespace AdminTool.Models;

// ─── Configuration ───────────────────────────────────────────────────────────

public class MakgaConfig
{
    public DbConfig Db { get; set; } = new();
    public RedisConfig Redis { get; set; } = new();
    public List<ServerInfo> Servers { get; set; } = new();
}

public class DbConfig
{
    public string Host { get; set; } = "127.0.0.1";
    public int Port { get; set; } = 3306;
    public string User { get; set; } = "root";
    public string Password { get; set; } = "";
    public string Database { get; set; } = "makga";

    public string ConnectionString =>
        $"Server={Host};Port={Port};Database={Database};User={User};Password={Password};" +
        "CharSet=utf8mb4;AllowZeroDateTime=true;ConvertZeroDateTime=true;";
}

public class RedisConfig
{
    public string Host { get; set; } = "127.0.0.1";
    public int Port { get; set; } = 6379;
    public string Password { get; set; } = "";
    public int Db { get; set; } = 0;
}

public class ServerInfo
{
    public string Name { get; set; } = "";
    public string Host { get; set; } = "";
    public int Port { get; set; } = 0;
}

// ─── DB Entities ─────────────────────────────────────────────────────────────

public class AdminAccount
{
    public int Id { get; set; }
    public string Username { get; set; } = "";
    public string PasswordHash { get; set; } = "";
    public string Role { get; set; } = "viewer";
    public bool IsActive { get; set; } = true;
    public DateTime? LastLogin { get; set; }
    public DateTime CreatedAt { get; set; }
}

// ─── Schema metadata ─────────────────────────────────────────────────────────

public class ColumnMeta
{
    public string Name { get; set; } = "";
    public string Type { get; set; } = "";
    public bool IsNullable { get; set; } = true;
    public bool IsPrimaryKey { get; set; } = false;
    public string? DefaultValue { get; set; }
    public bool IsAutoIncrement { get; set; } = false;

    public string InputType
    {
        get
        {
            var t = Type.ToLower();
            if (t == "tinyint(1)") return "checkbox";
            if (t.StartsWith("int") || t.StartsWith("bigint") || t.StartsWith("smallint") ||
                t.StartsWith("mediumint") || t.StartsWith("tinyint")) return "number";
            if (t.StartsWith("float") || t.StartsWith("double") || t.StartsWith("decimal")) return "number";
            if (t.StartsWith("datetime") || t.StartsWith("timestamp")) return "datetime-local";
            if (t.StartsWith("date")) return "date";
            return "text";
        }
    }

    public bool IsLongText => Type.ToLower().Contains("text") || Type.ToLower().Contains("blob");
}
