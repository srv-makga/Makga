using System.Globalization;
using OfficeOpenXml;
using TableExporter.Models;

namespace TableExporter.Core;

/// <summary>
/// .xlsx 파일을 읽어 TableData 목록으로 반환한다.
/// 각 시트 = 하나의 테이블. 첫 번째 행 = 컬럼 헤더.
/// </summary>
public static class ExcelReader
{
    public static List<TableData> Read(string path)
    {
        ExcelPackage.LicenseContext = LicenseContext.NonCommercial;

        var result = new List<TableData>();
        using var package = new ExcelPackage(new FileInfo(path));

        string fileName = Path.GetFileNameWithoutExtension(path);
        foreach (var sheet in package.Workbook.Worksheets)
        {
            var table = ReadSheet(sheet, fileName);
            if (table.Columns.Count > 0)
                result.Add(table);
        }

        return result;
    }

    private static TableData ReadSheet(ExcelWorksheet sheet, string fileName)
    {
        // 시트 이름이 기본값(Sheet1 등)이면 파일 이름을 테이블 이름으로 사용
        bool isDefaultName = sheet.Name.StartsWith("Sheet", StringComparison.OrdinalIgnoreCase)
                             && int.TryParse(sheet.Name["Sheet".Length..], out _);

        var table = new TableData
        {
            TableName = isDefaultName ? fileName : sheet.Name
        };

        if (sheet.Dimension == null)
            return table;

        int totalRows = sheet.Dimension.End.Row;
        int totalCols = sheet.Dimension.End.Column;

        // 첫 행: 컬럼 헤더
        for (int c = 1; c <= totalCols; c++)
        {
            string header = sheet.Cells[1, c].Text.Trim();
            if (string.IsNullOrEmpty(header))
                break;
            table.Columns.Add(header);
        }

        if (table.Columns.Count == 0)
            return table;

        // 나머지 행: 데이터
        for (int r = 2; r <= totalRows; r++)
        {
            // 완전히 빈 행 스킵
            bool hasData = false;
            for (int c = 1; c <= table.Columns.Count; c++)
            {
                if (sheet.Cells[r, c].Value != null)
                {
                    hasData = true;
                    break;
                }
            }
            if (!hasData) continue;

            var row = new List<string>();
            for (int c = 1; c <= table.Columns.Count; c++)
                row.Add(GetCellText(sheet.Cells[r, c]));

            table.Rows.Add(row);
        }

        return table;
    }

    /// <summary>
    /// 셀 값을 타입에 관계없이 일관된 문자열로 반환한다.
    /// 숫자는 InvariantCulture 포맷, DateTime은 yyyy-MM-dd 형식.
    /// </summary>
    private static string GetCellText(ExcelRangeBase cell)
    {
        if (cell.Value == null)
            return string.Empty;

        return cell.Value switch
        {
            double d   => d.ToString(CultureInfo.InvariantCulture),
            float f    => f.ToString(CultureInfo.InvariantCulture),
            long l     => l.ToString(CultureInfo.InvariantCulture),
            int i      => i.ToString(CultureInfo.InvariantCulture),
            bool b     => b.ToString().ToLowerInvariant(),
            DateTime dt => dt.ToString("yyyy-MM-dd", CultureInfo.InvariantCulture),
            _          => cell.Value.ToString()?.Trim() ?? string.Empty
        };
    }
}
