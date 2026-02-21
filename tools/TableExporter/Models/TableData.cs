namespace TableExporter.Models;

/// <summary>
/// 하나의 테이블(시트)에 해당하는 데이터. 첫 번째 행을 컬럼 헤더로 사용한다.
/// </summary>
public class TableData
{
    public string TableName { get; set; } = string.Empty;
    public List<string> Columns { get; set; } = [];
    public List<List<string>> Rows { get; set; } = [];
}
