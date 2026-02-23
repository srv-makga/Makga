using System.Data;
using Dapper;
using MySqlConnector;
using AdminTool.Models;

namespace AdminTool.Services;

public class DbService
{
    private readonly string _connStr;

    public DbService(MakgaConfig config) => _connStr = config.Db.ConnectionString;

    private MySqlConnection Open() => new(_connStr);

    // ─── Generic Dapper helpers ───────────────────────────────────────────────

    public async Task<IEnumerable<T>> QueryAsync<T>(string sql, object? param = null)
    {
        using var conn = Open();
        return await conn.QueryAsync<T>(sql, param);
    }

    public async Task<T?> QuerySingleAsync<T>(string sql, object? param = null)
    {
        using var conn = Open();
        return await conn.QuerySingleOrDefaultAsync<T>(sql, param);
    }

    public async Task<int> ExecuteAsync(string sql, object? param = null)
    {
        using var conn = Open();
        return await conn.ExecuteAsync(sql, param);
    }

    public async Task<bool> PingAsync()
    {
        try
        {
            using var conn = Open();
            await conn.QuerySingleAsync<int>("SELECT 1");
            return true;
        }
        catch { return false; }
    }

    // ─── Schema helpers ───────────────────────────────────────────────────────

    public async Task<List<string>> GetTablesAsync()
    {
        var rows = await QueryAsync<string>(
            "SELECT table_name FROM information_schema.tables " +
            "WHERE table_schema = DATABASE() ORDER BY table_name");
        return rows.ToList();
    }

    public async Task<List<ColumnMeta>> GetColumnsAsync(string table)
    {
        using var conn = Open();
        var rows = await conn.QueryAsync(
            "SELECT COLUMN_NAME AS Name, COLUMN_TYPE AS Type, IS_NULLABLE AS IsNullable, " +
            "COLUMN_KEY AS ColumnKey, COLUMN_DEFAULT AS DefaultValue, EXTRA AS Extra " +
            "FROM information_schema.columns " +
            "WHERE table_schema = DATABASE() AND table_name = @table " +
            "ORDER BY ORDINAL_POSITION",
            new { table });

        return rows.Select(r => new ColumnMeta
        {
            Name = (string)r.Name,
            Type = (string)r.Type,
            IsNullable = (string)r.IsNullable == "YES",
            IsPrimaryKey = (string)r.ColumnKey == "PRI",
            DefaultValue = r.DefaultValue?.ToString(),
            IsAutoIncrement = ((string?)r.Extra ?? "").Contains("auto_increment"),
        }).ToList();
    }

    // ─── Generic table data ───────────────────────────────────────────────────

    public async Task<(List<ColumnMeta> Columns, List<Dictionary<string, object?>> Rows, int Total)>
        GetTableDataAsync(string table, string? search, int page, int pageSize)
    {
        var validTables = await GetTablesAsync();
        if (!validTables.Contains(table, StringComparer.OrdinalIgnoreCase))
            return ([], [], 0);

        var columns = await GetColumnsAsync(table);
        var whereClause = "";
        var searchVal = $"%{search}%";

        if (!string.IsNullOrWhiteSpace(search))
        {
            var textCols = columns
                .Where(c => c.Type.Contains("char") || c.Type.Contains("text"))
                .ToList();
            if (textCols.Count > 0)
            {
                var clauses = textCols.Select(c => $"CAST(`{c.Name}` AS CHAR) LIKE @search");
                whereClause = $" WHERE {string.Join(" OR ", clauses)}";
            }
        }

        await using var conn = Open();
        await conn.OpenAsync();

        int total;
        await using (var cmd = conn.CreateCommand())
        {
            cmd.CommandText = $"SELECT COUNT(*) FROM `{table}`{whereClause}";
            if (!string.IsNullOrEmpty(whereClause)) cmd.Parameters.AddWithValue("@search", searchVal);
            total = Convert.ToInt32(await cmd.ExecuteScalarAsync());
        }

        var rows = new List<Dictionary<string, object?>>();
        await using (var cmd = conn.CreateCommand())
        {
            cmd.CommandText = $"SELECT * FROM `{table}`{whereClause} LIMIT @pageSize OFFSET @offset";
            if (!string.IsNullOrEmpty(whereClause)) cmd.Parameters.AddWithValue("@search", searchVal);
            cmd.Parameters.AddWithValue("@pageSize", pageSize);
            cmd.Parameters.AddWithValue("@offset", (page - 1) * pageSize);

            await using var reader = await cmd.ExecuteReaderAsync();
            while (await reader.ReadAsync())
            {
                var row = new Dictionary<string, object?>();
                for (int i = 0; i < reader.FieldCount; i++)
                    row[reader.GetName(i)] = reader.IsDBNull(i) ? null : reader.GetValue(i);
                rows.Add(row);
            }
        }

        return (columns, rows, total);
    }

    // ─── DML ─────────────────────────────────────────────────────────────────

    public async Task UpdateRowAsync(string table, Dictionary<string, string?> values,
        string pkCol, string pkValue)
    {
        var validTables = await GetTablesAsync();
        if (!validTables.Contains(table, StringComparer.OrdinalIgnoreCase))
            throw new InvalidOperationException($"Table '{table}' not found.");

        var columns = await GetColumnsAsync(table);
        var validColNames = columns.Select(c => c.Name).ToHashSet(StringComparer.OrdinalIgnoreCase);

        var sets = new List<string>();
        await using var conn = Open();
        await conn.OpenAsync();
        await using var cmd = conn.CreateCommand();

        int i = 0;
        foreach (var (col, val) in values)
        {
            if (!validColNames.Contains(col) || col.Equals(pkCol, StringComparison.OrdinalIgnoreCase)) continue;
            sets.Add($"`{col}` = @p{i}");
            cmd.Parameters.AddWithValue($"@p{i}", string.IsNullOrEmpty(val) ? DBNull.Value : (object)val);
            i++;
        }
        if (sets.Count == 0) return;

        cmd.Parameters.AddWithValue("@pk", pkValue);
        cmd.CommandText = $"UPDATE `{table}` SET {string.Join(", ", sets)} WHERE `{pkCol}` = @pk";
        await cmd.ExecuteNonQueryAsync();
    }

    public async Task InsertRowAsync(string table, Dictionary<string, string?> values)
    {
        var validTables = await GetTablesAsync();
        if (!validTables.Contains(table, StringComparer.OrdinalIgnoreCase))
            throw new InvalidOperationException($"Table '{table}' not found.");

        var columns = await GetColumnsAsync(table);
        var insertableCols = columns
            .Where(c => !c.IsAutoIncrement)
            .Select(c => c.Name)
            .ToHashSet(StringComparer.OrdinalIgnoreCase);

        var colNames = new List<string>();
        var paramNames = new List<string>();
        await using var conn = Open();
        await conn.OpenAsync();
        await using var cmd = conn.CreateCommand();

        int i = 0;
        foreach (var (col, val) in values)
        {
            if (!insertableCols.Contains(col)) continue;
            colNames.Add($"`{col}`");
            paramNames.Add($"@p{i}");
            cmd.Parameters.AddWithValue($"@p{i}", string.IsNullOrEmpty(val) ? DBNull.Value : (object)val);
            i++;
        }
        if (colNames.Count == 0) return;

        cmd.CommandText = $"INSERT INTO `{table}` ({string.Join(", ", colNames)}) VALUES ({string.Join(", ", paramNames)})";
        await cmd.ExecuteNonQueryAsync();
    }

    public async Task DeleteRowAsync(string table, string pkCol, string pkValue)
    {
        var validTables = await GetTablesAsync();
        if (!validTables.Contains(table, StringComparer.OrdinalIgnoreCase))
            throw new InvalidOperationException($"Table '{table}' not found.");

        var columns = await GetColumnsAsync(table);
        if (!columns.Any(c => c.Name.Equals(pkCol, StringComparison.OrdinalIgnoreCase)))
            throw new InvalidOperationException($"Column '{pkCol}' not found.");

        await using var conn = Open();
        await conn.OpenAsync();
        await using var cmd = conn.CreateCommand();
        cmd.CommandText = $"DELETE FROM `{table}` WHERE `{pkCol}` = @pk";
        cmd.Parameters.AddWithValue("@pk", pkValue);
        await cmd.ExecuteNonQueryAsync();
    }
}
