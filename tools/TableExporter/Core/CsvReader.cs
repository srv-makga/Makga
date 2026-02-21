using TableExporter.Models;

namespace TableExporter.Core;

/// <summary>
/// .csv 파일을 읽어 TableData로 반환한다. 큰따옴표 이스케이프를 지원한다.
/// </summary>
public static class CsvReader
{
    public static TableData Read(string path)
    {
        var table = new TableData
        {
            TableName = Path.GetFileNameWithoutExtension(path)
        };

        var lines = File.ReadAllLines(path, System.Text.Encoding.UTF8);
        if (lines.Length == 0)
            return table;

        table.Columns.AddRange(ParseLine(lines[0]));

        for (int i = 1; i < lines.Length; i++)
        {
            if (string.IsNullOrWhiteSpace(lines[i]))
                continue;
            table.Rows.Add(ParseLine(lines[i]));
        }

        return table;
    }

    private static List<string> ParseLine(string line)
    {
        var fields = new List<string>();
        bool inQuotes = false;
        var current = new System.Text.StringBuilder();

        for (int i = 0; i < line.Length; i++)
        {
            char c = line[i];
            if (c == '"')
            {
                if (inQuotes && i + 1 < line.Length && line[i + 1] == '"')
                {
                    current.Append('"');
                    i++;
                }
                else
                {
                    inQuotes = !inQuotes;
                }
            }
            else if (c == ',' && !inQuotes)
            {
                fields.Add(current.ToString());
                current.Clear();
            }
            else
            {
                current.Append(c);
            }
        }

        fields.Add(current.ToString());
        return fields;
    }
}
