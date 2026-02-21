# TableExporter

Excel(`.xlsx`) 파일을 원본으로 하는 게임 데이터 테이블을 **CSV / JSON / SQL INSERT** 형식으로 추출하는 C# 콘솔 도구입니다.
반대로 기존 CSV 파일을 Excel 형식으로 변환하는 `import` 기능도 제공합니다.

---

## 빌드

```
dotnet build TableExporter.csproj -c Release
```

빌드 결과물: `bin\Release\net8.0\TableExporter.exe`

---

## 사용법

```
TableExporter <command> [options]
```

### `export` — Excel → CSV / JSON / SQL

| 옵션 | 설명 | 기본값 |
|------|------|--------|
| `--input <path>` | `.xlsx` 파일 또는 폴더 경로 | 필수 |
| `--output <dir>` | 출력 디렉토리 | 필수 |
| `--format <fmt>` | `csv` \| `json` \| `sql` \| `all` | `csv` |
| `--dialect <db>` | SQL 방언: `mysql` \| `mssql` \| `sqlite` \| `postgresql` | `mysql` |
| `--no-schema` | SQL 출력에서 `CREATE TABLE` 구문 생략 | (포함) |

### `import` — CSV → Excel

| 옵션 | 설명 | 기본값 |
|------|------|--------|
| `--input <path>` | `.csv` 파일 또는 폴더 경로 | 필수 |
| `--output <dir>` | 출력 디렉토리 | 필수 |
| `--merge` | 모든 CSV를 하나의 xlsx 파일(멀티 시트)로 병합 | 개별 파일 |

---

## 예시

```bash
# 1. 기존 CSV 데이터를 Excel로 변환 (최초 1회 — 이후 xlsx가 원본)
TableExporter import --input ..\..\tables\csv --output ..\..\tables\excel

# 2. 모든 CSV를 하나의 멀티시트 xlsx로 병합
TableExporter import --input ..\..\tables\csv --output ..\..\tables\excel --merge

# 3. Excel → CSV 내보내기
TableExporter export --input ..\..\tables\excel --output ..\..\tables\csv --format csv

# 4. Excel → JSON 내보내기
TableExporter export --input ..\..\tables\excel --output ..\..\tables\json --format json

# 5. Excel → MySQL SQL 내보내기 (CREATE TABLE + INSERT)
TableExporter export --input ..\..\tables\excel --output ..\..\tables\sql --format sql

# 6. Excel → MS-SQL SQL, INSERT 만 출력
TableExporter export --input ..\..\tables\excel --output ..\..\tables\sql --format sql --dialect mssql --no-schema

# 7. 한 번에 csv/json/sql 세 가지 모두 내보내기
TableExporter export --input ..\..\tables\excel --output ..\..\tables\out --format all

# 8. 단일 파일 변환
TableExporter export --input ..\..\tables\excel\skill.xlsx --output .\out --format json
```

---

## Excel 파일 규칙

- **첫 번째 행**: 컬럼 헤더 (영문 스네이크케이스 권장, e.g. `skill_id`)
- **두 번째 행 이후**: 데이터
- **시트 이름**: 테이블 이름으로 사용됨 (기본 `Sheet1`이면 파일명으로 대체)
- **멀티 시트**: 하나의 xlsx 파일에 여러 시트가 있을 경우 시트 수만큼 테이블을 내보냄

---

## 타입 추론 (SQL 출력 시)

SQL 내보내기 시 컬럼의 모든 값을 분석하여 타입을 자동 추론합니다.

| 감지 조건 | MySQL | MSSQL | SQLite | PostgreSQL |
|-----------|-------|-------|--------|------------|
| 모든 값이 정수 | `INT` | `INT` | `INTEGER` | `INTEGER` |
| 모든 값이 실수 | `FLOAT` | `FLOAT` | `REAL` | `DOUBLE PRECISION` |
| `true`/`false` | `TINYINT(1)` | `BIT` | `INTEGER` | `BOOLEAN` |
| 그 외 | `VARCHAR(512)` | `NVARCHAR(512)` | `TEXT` | `VARCHAR(512)` |

첫 번째 컬럼이 `id` 또는 `*_id` 형식이고 정수형이면 `PRIMARY KEY`를 자동 지정합니다.

---

## 의존성

- [EPPlus 7.x](https://epplussoftware.com/) — Excel 읽기/쓰기 (`LicenseContext.NonCommercial` 사용)
- .NET 8
