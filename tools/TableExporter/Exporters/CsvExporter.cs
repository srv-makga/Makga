using System.Text;
using TableExporter.Models;

namespace TableExporter.Exporters;

public class CsvExporter : IExporter
{
    public void Export(TableData table, string outputDir)
    {
        var path = Path.Combine(outputDir, $"{table.TableName}.csv");
        using var writer = new StreamWriter(path, false, new UTF8Encoding(encoderShouldEmitUTF8Identifier: true));

        writer.WriteLine(string.Join(",", table.Columns.Select(Escape)));
        foreach (var row in table.Rows)
            writer.WriteLine(string.Join(",", row.Select(v => Escape(v))));

        Console.WriteLine($"  [CSV]  {path}");
    }

    private static string Escape(string value)
    {
        if (value.Contains(',') || value.Contains('"') || value.Contains('\n') || value.Contains('\r'))
            return $"\"{value.Replace("\"", "\"\"")}\"";
        return value;
    }
}
