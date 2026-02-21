using OfficeOpenXml;
using OfficeOpenXml.Style;
using TableExporter.Models;

namespace TableExporter.Core;

/// <summary>
/// TableData를 .xlsx 파일로 저장한다.
/// </summary>
public static class ExcelWriter
{
    /// <summary>여러 TableData를 하나의 xlsx 파일(멀티 시트)로 저장한다.</summary>
    public static void Write(IEnumerable<TableData> tables, string outputPath)
    {
        ExcelPackage.LicenseContext = LicenseContext.NonCommercial;

        using var package = new ExcelPackage();
        foreach (var table in tables)
            ApplySheet(package.Workbook.Worksheets.Add(table.TableName), table);

        package.SaveAs(new FileInfo(outputPath));
        Console.WriteLine($"  [XLSX] {outputPath}");
    }

    /// <summary>단일 TableData를 xlsx 파일로 저장한다.</summary>
    public static void WriteOne(TableData table, string outputPath)
    {
        ExcelPackage.LicenseContext = LicenseContext.NonCommercial;

        using var package = new ExcelPackage();
        ApplySheet(package.Workbook.Worksheets.Add(table.TableName), table);
        package.SaveAs(new FileInfo(outputPath));
        Console.WriteLine($"  [XLSX] {outputPath}");
    }

    private static void ApplySheet(ExcelWorksheet sheet, TableData table)
    {
        // 헤더 행
        for (int c = 0; c < table.Columns.Count; c++)
        {
            var cell = sheet.Cells[1, c + 1];
            cell.Value = table.Columns[c];
            cell.Style.Font.Bold = true;
            cell.Style.Fill.PatternType = ExcelFillStyle.Solid;
            // 헤더 배경색: 파란 계열 (ARGB)
            cell.Style.Fill.BackgroundColor.SetColor(255, 52, 120, 209);  // A=255 불투명
            cell.Style.Font.Color.SetColor(255, 255, 255, 255);
            cell.Style.HorizontalAlignment = ExcelHorizontalAlignment.Center;
        }

        // 데이터 행
        for (int r = 0; r < table.Rows.Count; r++)
        {
            var row = table.Rows[r];
            for (int c = 0; c < row.Count && c < table.Columns.Count; c++)
            {
                var cell = sheet.Cells[r + 2, c + 1];
                // 숫자 타입으로 저장 가능하면 숫자로 저장
                if (long.TryParse(row[c], out long lv))
                    cell.Value = lv;
                else if (double.TryParse(row[c], System.Globalization.NumberStyles.Float,
                         System.Globalization.CultureInfo.InvariantCulture, out double dv))
                    cell.Value = dv;
                else
                    cell.Value = row[c];
            }

            // 짝수 행 배경색 (가독성)
            if (r % 2 == 1)
            {
                var rowRange = sheet.Cells[r + 2, 1, r + 2, table.Columns.Count];
                rowRange.Style.Fill.PatternType = ExcelFillStyle.Solid;
                rowRange.Style.Fill.BackgroundColor.SetColor(255, 242, 242, 242);
            }
        }

        // 열 너비 자동 조절
        if (sheet.Dimension != null)
            sheet.Cells[sheet.Dimension.Address].AutoFitColumns(8, 60);

        // 헤더 행 고정 (스크롤 시 헤더 유지)
        sheet.View.FreezePanes(2, 1);
    }
}
