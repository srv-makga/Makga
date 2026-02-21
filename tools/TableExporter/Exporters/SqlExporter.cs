using System.Globalization;
using System.Text;
using TableExporter.Models;

namespace TableExporter.Exporters;

public enum SqlDialect { MySQL, MSSQL, SQLite, PostgreSQL }

public class SqlExporter : IExporter
{
    private readonly SqlDialect _dialect;
    private readonly bool _generateSchema;

    public SqlExporter(SqlDialect dialect = SqlDialect.MySQL, bool generateSchema = true)
    {
        _dialect = dialect;
        _generateSchema = generateSchema;
    }

    public void Export(TableData table, string outputDir)
    {
        var path = Path.Combine(outputDir, $"{table.TableName}.sql");
        var sb = new StringBuilder();

        sb.AppendLine($"-- ============================================================");
        sb.AppendLine($"-- Table : {table.TableName}");
        sb.AppendLine($"-- Rows  : {table.Rows.Count}");
        sb.AppendLine($"-- Generated: {DateTime.Now:yyyy-MM-dd HH:mm:ss}");
        sb.AppendLine($"-- ============================================================");
        sb.AppendLine();

        if (_generateSchema)
        {
            AppendCreateTable(sb, table);
            sb.AppendLine();
        }

        AppendInserts(sb, table);

        File.WriteAllText(path, sb.ToString(), new UTF8Encoding(encoderShouldEmitUTF8Identifier: false));
        Console.WriteLine($"  [SQL]  {path}");
    }

    // ── CREATE TABLE ─────────────────────────────────────────────────────────

    private void AppendCreateTable(StringBuilder sb, TableData table)
    {
        var colTypes = DetectColumnTypes(table);

        string tableName = WrapName(table.TableName);
        sb.AppendLine(_dialect switch
        {
            SqlDialect.MySQL      => $"CREATE TABLE IF NOT EXISTS {tableName} (",
            SqlDialect.MSSQL      => $"IF OBJECT_ID(N'{table.TableName}', N'U') IS NULL\nCREATE TABLE {tableName} (",
            SqlDialect.SQLite     => $"CREATE TABLE IF NOT EXISTS {tableName} (",
            SqlDialect.PostgreSQL => $"CREATE TABLE IF NOT EXISTS {tableName} (",
            _                     => $"CREATE TABLE IF NOT EXISTS {tableName} ("
        });

        for (int i = 0; i < table.Columns.Count; i++)
        {
            string col = WrapName(table.Columns[i]);
            string type = MapType(colTypes[i]);
            bool isLast = i == table.Columns.Count - 1;

            // 첫 번째 컬럼이 id / *_id 형식이면 PRIMARY KEY 지정
            bool isPk = i == 0 &&
                        (table.Columns[i].Equals("id", StringComparison.OrdinalIgnoreCase) ||
                         table.Columns[i].EndsWith("_id", StringComparison.OrdinalIgnoreCase));

            string pk = (isPk && IsIntType(colTypes[i])) ? " PRIMARY KEY" : "";
            sb.AppendLine($"  {col} {type}{pk}{(isLast ? "" : ",")}");
        }

        sb.AppendLine(");");
    }

    // ── INSERT INTO ──────────────────────────────────────────────────────────

    private void AppendInserts(StringBuilder sb, TableData table)
    {
        if (table.Rows.Count == 0)
            return;

        string tableName = WrapName(table.TableName);
        string colList   = string.Join(", ", table.Columns.Select(WrapName));

        foreach (var row in table.Rows)
        {
            var values = row.Select((v, i) => QuoteValue(v)).ToList();
            // 행이 컬럼 수보다 짧을 경우 NULL 패딩
            while (values.Count < table.Columns.Count)
                values.Add("NULL");

            sb.AppendLine($"INSERT INTO {tableName} ({colList}) VALUES ({string.Join(", ", values)});");
        }
    }

    // ── 타입 추론 ─────────────────────────────────────────────────────────────

    private enum ColType { Int, Float, Bool, Text }

    private List<ColType> DetectColumnTypes(TableData table)
    {
        var types = Enumerable.Repeat(ColType.Int, table.Columns.Count).ToList();

        foreach (var row in table.Rows)
        {
            for (int c = 0; c < table.Columns.Count && c < row.Count; c++)
            {
                string v = row[c];
                if (string.IsNullOrEmpty(v)) continue;

                if (types[c] == ColType.Int)
                {
                    if (v.Equals("true", StringComparison.OrdinalIgnoreCase) ||
                        v.Equals("false", StringComparison.OrdinalIgnoreCase))
                        types[c] = ColType.Bool;
                    else if (!long.TryParse(v, out _))
                        types[c] = double.TryParse(v, NumberStyles.Float,
                            CultureInfo.InvariantCulture, out _)
                            ? ColType.Float
                            : ColType.Text;
                }
                else if (types[c] == ColType.Float)
                {
                    if (!double.TryParse(v, NumberStyles.Float,
                        CultureInfo.InvariantCulture, out _))
                        types[c] = ColType.Text;
                }
            }
        }

        return types;
    }

    private string MapType(ColType t) => (_dialect, t) switch
    {
        (SqlDialect.MSSQL, ColType.Int)   => "INT",
        (SqlDialect.MSSQL, ColType.Float) => "FLOAT",
        (SqlDialect.MSSQL, ColType.Bool)  => "BIT",
        (SqlDialect.MSSQL, ColType.Text)  => "NVARCHAR(512)",
        (SqlDialect.SQLite, ColType.Int)  => "INTEGER",
        (SqlDialect.SQLite, ColType.Float)=> "REAL",
        (SqlDialect.SQLite, ColType.Bool) => "INTEGER",
        (SqlDialect.SQLite, ColType.Text) => "TEXT",
        (SqlDialect.PostgreSQL, ColType.Int)   => "INTEGER",
        (SqlDialect.PostgreSQL, ColType.Float) => "DOUBLE PRECISION",
        (SqlDialect.PostgreSQL, ColType.Bool)  => "BOOLEAN",
        (SqlDialect.PostgreSQL, ColType.Text)  => "VARCHAR(512)",
        // MySQL (default)
        (_, ColType.Int)   => "INT",
        (_, ColType.Float) => "FLOAT",
        (_, ColType.Bool)  => "TINYINT(1)",
        _                  => "VARCHAR(512)"
    };

    private static bool IsIntType(ColType t) => t is ColType.Int or ColType.Bool;

    // ── 헬퍼 ──────────────────────────────────────────────────────────────────

    private string WrapName(string name) => _dialect switch
    {
        SqlDialect.MSSQL      => $"[{name}]",
        SqlDialect.PostgreSQL => $"\"{name}\"",
        _                     => $"`{name}`"
    };

    private static string QuoteValue(string value)
    {
        if (string.IsNullOrEmpty(value))
            return "NULL";
        if (value.Equals("true",  StringComparison.OrdinalIgnoreCase)) return "1";
        if (value.Equals("false", StringComparison.OrdinalIgnoreCase)) return "0";
        if (long.TryParse(value, out _)) return value;
        if (double.TryParse(value, NumberStyles.Float, CultureInfo.InvariantCulture, out _))
            return value;
        return $"'{value.Replace("'", "''")}'";
    }
}
