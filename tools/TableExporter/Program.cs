using TableExporter.Core;
using TableExporter.Exporters;
using TableExporter.Models;

// ─────────────────────────────────────────────────────────────────────────────
//  TableExporter — 게임 데이터 테이블 변환 도구
//  Commands:
//    export  : Excel(.xlsx) → CSV / JSON / SQL INSERT 쿼리
//    import  : CSV(.csv)    → Excel(.xlsx)
// ─────────────────────────────────────────────────────────────────────────────

if (args.Length == 0 || args[0] is "-h" or "--help")
{
    PrintHelp();
    return 0;
}

return args[0].ToLower() switch
{
    "export" => HandleExport(args[1..]),
    "import" => HandleImport(args[1..]),
    _        => HandleUnknown(args[0])
};

// ── export ────────────────────────────────────────────────────────────────────

static int HandleExport(string[] args)
{
    string? inputPath = null;
    string? outputDir = null;
    string  format    = "csv";
    SqlDialect dialect = SqlDialect.MySQL;
    bool   schema     = true;

    for (int i = 0; i < args.Length; i++)
    {
        switch (args[i].ToLower())
        {
            case "--input":   inputPath = args[++i]; break;
            case "--output":  outputDir = args[++i]; break;
            case "--format":  format    = args[++i].ToLower(); break;
            case "--no-schema": schema  = false; break;
            case "--dialect":
                dialect = args[++i].ToLower() switch
                {
                    "mssql"      => SqlDialect.MSSQL,
                    "sqlite"     => SqlDialect.SQLite,
                    "postgresql" => SqlDialect.PostgreSQL,
                    _            => SqlDialect.MySQL
                };
                break;
        }
    }

    if (inputPath is null || outputDir is null)
    {
        Console.Error.WriteLine("오류: --input, --output 옵션이 필요합니다.");
        Console.Error.WriteLine("도움말: TableExporter --help");
        return 1;
    }

    // 입력 파일 목록 수집
    string[] files = GetFiles(inputPath, "*.xlsx");
    if (files.Length == 0)
    {
        Console.Error.WriteLine($"xlsx 파일을 찾을 수 없습니다: {inputPath}");
        return 1;
    }

    // format=all 이면 csv/json/sql 세 폴더에 각각 출력
    string[] formats = format == "all" ? ["csv", "json", "sql"] : [format];

    int totalTables = 0;
    foreach (string file in files.OrderBy(f => f))
    {
        Console.WriteLine($"읽는 중: {Path.GetFileName(file)}");
        List<TableData> tables = ExcelReader.Read(file);

        foreach (TableData table in tables)
        {
            foreach (string fmt in formats)
            {
                string outDir = formats.Length > 1
                    ? Path.Combine(outputDir, fmt)
                    : outputDir;
                Directory.CreateDirectory(outDir);

                IExporter exporter = fmt switch
                {
                    "json" => new JsonExporter(),
                    "sql"  => new SqlExporter(dialect, schema),
                    _      => new CsvExporter()
                };

                exporter.Export(table, outDir);
            }
            totalTables++;
        }
    }

    Console.WriteLine($"\n완료. {totalTables}개 테이블을 [{outputDir}]에 저장했습니다.");
    return 0;
}

// ── import ────────────────────────────────────────────────────────────────────

static int HandleImport(string[] args)
{
    string? inputPath = null;
    string? outputDir = null;
    bool    merge     = false;

    for (int i = 0; i < args.Length; i++)
    {
        switch (args[i].ToLower())
        {
            case "--input":  inputPath = args[++i]; break;
            case "--output": outputDir = args[++i]; break;
            case "--merge":  merge = true; break;
        }
    }

    if (inputPath is null || outputDir is null)
    {
        Console.Error.WriteLine("오류: --input, --output 옵션이 필요합니다.");
        return 1;
    }

    Directory.CreateDirectory(outputDir);
    string[] csvFiles = GetFiles(inputPath, "*.csv");

    if (csvFiles.Length == 0)
    {
        Console.Error.WriteLine($"csv 파일을 찾을 수 없습니다: {inputPath}");
        return 1;
    }

    var tables = csvFiles.OrderBy(f => f).Select(CsvReader.Read).ToList();

    if (merge)
    {
        // 모든 CSV → 하나의 xlsx (멀티 시트)
        string outPath = Path.Combine(outputDir, "tables.xlsx");
        ExcelWriter.Write(tables, outPath);
        Console.WriteLine($"\n완료. {tables.Count}개 시트를 [{outPath}]에 병합했습니다.");
    }
    else
    {
        // CSV마다 개별 xlsx 파일
        foreach (TableData table in tables)
        {
            string outPath = Path.Combine(outputDir, $"{table.TableName}.xlsx");
            ExcelWriter.WriteOne(table, outPath);
        }
        Console.WriteLine($"\n완료. {tables.Count}개 파일을 [{outputDir}]에 변환했습니다.");
    }

    return 0;
}

// ── 유틸 ─────────────────────────────────────────────────────────────────────

static string[] GetFiles(string path, string pattern) =>
    Directory.Exists(path)
        ? Directory.GetFiles(path, pattern)
        : File.Exists(path) ? [path] : [];

static int HandleUnknown(string command)
{
    Console.Error.WriteLine($"알 수 없는 명령어: '{command}'");
    PrintHelp();
    return 1;
}

static void PrintHelp()
{
    Console.WriteLine("""
    ┌─────────────────────────────────────────────────────────────────┐
    │  TableExporter  —  게임 데이터 테이블 변환 도구                   │
    └─────────────────────────────────────────────────────────────────┘

    사용법:
      TableExporter <command> [options]

    ── Commands ──────────────────────────────────────────────────────
      export   Excel(.xlsx) → CSV / JSON / SQL 변환
      import   CSV(.csv)    → Excel(.xlsx) 변환

    ── export 옵션 ───────────────────────────────────────────────────
      --input     <path>   .xlsx 파일 또는 폴더 경로  [필수]
      --output    <dir>    출력 디렉토리               [필수]
      --format    <fmt>    csv | json | sql | all      [기본: csv]
      --dialect   <db>     (sql 전용) mysql | mssql | sqlite | postgresql
                           [기본: mysql]
      --no-schema          CREATE TABLE 구문 생략 (INSERT 만 출력)

    ── import 옵션 ───────────────────────────────────────────────────
      --input     <path>   .csv 파일 또는 폴더 경로   [필수]
      --output    <dir>    출력 디렉토리               [필수]
      --merge              모든 CSV → 하나의 xlsx (멀티 시트)

    ── 사용 예시 ─────────────────────────────────────────────────────
      # CSV 내보내기
      TableExporter export --input tables\excel --output tables\csv --format csv

      # JSON 내보내기
      TableExporter export --input tables\excel --output tables\json --format json

      # SQL 내보내기 (MySQL, CREATE TABLE + INSERT 포함)
      TableExporter export --input tables\excel --output tables\sql --format sql

      # SQL 내보내기 (MS-SQL, INSERT 만)
      TableExporter export --input tables\excel --output tables\sql --format sql --dialect mssql --no-schema

      # 한 번에 csv/json/sql 모두 내보내기
      TableExporter export --input tables\excel --output tables\out --format all

      # CSV → 개별 xlsx 변환
      TableExporter import --input tables\csv --output tables\excel

      # CSV → 하나의 xlsx (멀티 시트) 병합
      TableExporter import --input tables\csv --output tables\excel --merge
    """);
}
