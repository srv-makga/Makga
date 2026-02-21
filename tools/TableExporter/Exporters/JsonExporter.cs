using System.Text;
using System.Text.Encodings.Web;
using System.Text.Json;
using System.Text.Unicode;
using TableExporter.Models;

namespace TableExporter.Exporters;

public class JsonExporter : IExporter
{
    public void Export(TableData table, string outputDir)
    {
        var path = Path.Combine(outputDir, $"{table.TableName}.json");

        var records = table.Rows.Select(row =>
        {
            var dict = new Dictionary<string, object?>();
            for (int i = 0; i < table.Columns.Count && i < row.Count; i++)
            {
                string val = row[i];
                // 타입 추론: 빈 값, 숫자, 불리언, 문자열 순서로 판별
                dict[table.Columns[i]] = InferValue(val);
            }
            return dict;
        }).ToList();

        var options = new JsonSerializerOptions
        {
            WriteIndented = true,
            // 한글 등 유니코드를 이스케이프 없이 출력
            Encoder = JavaScriptEncoder.Create(UnicodeRanges.All)
        };

        var json = JsonSerializer.Serialize(records, options);
        File.WriteAllText(path, json, new UTF8Encoding(encoderShouldEmitUTF8Identifier: false));

        Console.WriteLine($"  [JSON] {path}");
    }

    private static object? InferValue(string value)
    {
        if (string.IsNullOrEmpty(value))
            return null;
        if (value.Equals("true", StringComparison.OrdinalIgnoreCase))
            return true;
        if (value.Equals("false", StringComparison.OrdinalIgnoreCase))
            return false;
        if (long.TryParse(value, out long lv))
            return lv;
        if (double.TryParse(value, System.Globalization.NumberStyles.Float,
            System.Globalization.CultureInfo.InvariantCulture, out double dv))
            return dv;
        return value;
    }
}
