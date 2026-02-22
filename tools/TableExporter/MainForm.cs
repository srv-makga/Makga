using System.Drawing;
using System.Windows.Forms;
using TableExporter.Core;
using TableExporter.Exporters;
using TableExporter.Models;

namespace TableExporter;

public class MainForm : Form
{
    // ── Controls ─────────────────────────────────────────────────────────────
    private readonly TextBox      _excelFolderBox;
    private readonly CheckedListBox _fileList;
    private readonly CheckBox     _chkCsv;
    private readonly CheckBox     _chkJson;
    private readonly CheckBox     _chkSql;
    private readonly ComboBox     _dialectCombo;
    private readonly CheckBox     _chkSchema;
    private readonly CheckBox     _chkSubFolders;
    private readonly TextBox      _outputFolderBox;
    private readonly Button       _btnExport;
    private readonly TextBox      _logBox;
    private readonly Label        _lblStatus;

    public MainForm()
    {
        Text            = "TableExporter — 게임 데이터 테이블 변환 도구";
        Size            = new Size(780, 720);
        MinimumSize     = new Size(680, 620);
        StartPosition   = FormStartPosition.CenterScreen;
        Font            = new Font("Segoe UI", 9f);

        // ── Excel 폴더 GroupBox ──────────────────────────────────────────────
        var grpExcel = new GroupBox { Text = "Excel 파일", Dock = DockStyle.Top, Height = 230, Padding = new Padding(8) };

        var pnlExcelPath = new FlowLayoutPanel
        {
            Dock = DockStyle.Top, Height = 34, FlowDirection = FlowDirection.LeftToRight,
            WrapContents = false, AutoSize = false
        };
        var lblFolder = new Label { Text = "폴더:", AutoSize = true, Margin = new Padding(0, 6, 4, 0) };
        _excelFolderBox = new TextBox { Width = 380, Margin = new Padding(0, 3, 4, 0) };
        var btnBrowseExcel = MakeButton("찾아보기", 72);
        var btnRefresh     = MakeButton("새로고침", 72);
        pnlExcelPath.Controls.AddRange([lblFolder, _excelFolderBox, btnBrowseExcel, btnRefresh]);

        var pnlSelectAll = new FlowLayoutPanel
        {
            Dock = DockStyle.Top, Height = 28, FlowDirection = FlowDirection.LeftToRight,
            WrapContents = false
        };
        var btnSelectAll   = MakeButton("전체 선택", 72);
        var btnDeselectAll = MakeButton("전체 해제", 72);
        pnlSelectAll.Controls.AddRange([btnSelectAll, btnDeselectAll]);

        _fileList = new CheckedListBox
        {
            Dock = DockStyle.Fill, CheckOnClick = true,
            IntegralHeight = false, BorderStyle = BorderStyle.FixedSingle
        };

        grpExcel.Controls.Add(_fileList);
        grpExcel.Controls.Add(pnlSelectAll);
        grpExcel.Controls.Add(pnlExcelPath);

        // ── 출력 형식 GroupBox ───────────────────────────────────────────────
        var grpFormat = new GroupBox { Text = "출력 형식", Dock = DockStyle.Top, Height = 88, Padding = new Padding(8) };

        var pnlFormats = new FlowLayoutPanel
        {
            Dock = DockStyle.Top, Height = 30, FlowDirection = FlowDirection.LeftToRight,
            WrapContents = false
        };
        _chkCsv  = new CheckBox { Text = "CSV",        Checked = true,  AutoSize = true, Margin = new Padding(0, 4, 16, 0) };
        _chkJson = new CheckBox { Text = "JSON",       Checked = false, AutoSize = true, Margin = new Padding(0, 4, 16, 0) };
        _chkSql  = new CheckBox { Text = "SQL (Query)",Checked = false, AutoSize = true, Margin = new Padding(0, 4, 0, 0) };
        pnlFormats.Controls.AddRange([_chkCsv, _chkJson, _chkSql]);

        var pnlSqlOpts = new FlowLayoutPanel
        {
            Dock = DockStyle.Top, Height = 30, FlowDirection = FlowDirection.LeftToRight,
            WrapContents = false
        };
        var lblDialect = new Label { Text = "SQL 방언:", AutoSize = true, Margin = new Padding(0, 6, 6, 0) };
        _dialectCombo  = new ComboBox
        {
            DropDownStyle = ComboBoxStyle.DropDownList, Width = 130,
            Margin = new Padding(0, 3, 16, 0)
        };
        _dialectCombo.Items.AddRange(["MySQL", "MSSQL", "SQLite", "PostgreSQL"]);
        _dialectCombo.SelectedIndex = 0;
        _chkSchema = new CheckBox { Text = "CREATE TABLE 포함", Checked = true, AutoSize = true, Margin = new Padding(0, 4, 0, 0) };
        pnlSqlOpts.Controls.AddRange([lblDialect, _dialectCombo, _chkSchema]);

        grpFormat.Controls.Add(pnlSqlOpts);
        grpFormat.Controls.Add(pnlFormats);

        // ── 출력 폴더 GroupBox ───────────────────────────────────────────────
        var grpOutput = new GroupBox { Text = "출력 폴더", Dock = DockStyle.Top, Height = 86, Padding = new Padding(8) };

        var pnlOutputPath = new FlowLayoutPanel
        {
            Dock = DockStyle.Top, Height = 34, FlowDirection = FlowDirection.LeftToRight,
            WrapContents = false
        };
        var lblOut = new Label { Text = "폴더:", AutoSize = true, Margin = new Padding(0, 6, 4, 0) };
        _outputFolderBox = new TextBox { Width = 400, Margin = new Padding(0, 3, 4, 0) };
        var btnBrowseOut = MakeButton("찾아보기", 72);
        pnlOutputPath.Controls.AddRange([lblOut, _outputFolderBox, btnBrowseOut]);

        var pnlOutOpts = new FlowLayoutPanel
        {
            Dock = DockStyle.Top, Height = 28, FlowDirection = FlowDirection.LeftToRight
        };
        _chkSubFolders = new CheckBox
        {
            Text = "형식별 하위 폴더로 분리 (csv/ json/ sql/)",
            Checked = true, AutoSize = true, Margin = new Padding(0, 4, 0, 0)
        };
        pnlOutOpts.Controls.Add(_chkSubFolders);

        grpOutput.Controls.Add(pnlOutOpts);
        grpOutput.Controls.Add(pnlOutputPath);

        // ── 내보내기 버튼 ────────────────────────────────────────────────────
        var pnlAction = new Panel { Dock = DockStyle.Top, Height = 42 };
        _btnExport = new Button
        {
            Text = "내보내기", Width = 120, Height = 32,
            Font = new Font("Segoe UI", 10f, FontStyle.Bold),
            BackColor = Color.FromArgb(0, 120, 212), ForeColor = Color.White,
            FlatStyle = FlatStyle.Flat
        };
        _btnExport.FlatAppearance.BorderSize = 0;
        _btnExport.Anchor = AnchorStyles.Right;
        _btnExport.Location = new Point(pnlAction.Width - 132, 5);
        pnlAction.Controls.Add(_btnExport);

        // ── 로그 영역 ────────────────────────────────────────────────────────
        var grpLog = new GroupBox { Text = "로그", Dock = DockStyle.Fill, Padding = new Padding(8) };
        _logBox = new TextBox
        {
            Dock = DockStyle.Fill, Multiline = true, ReadOnly = true,
            ScrollBars = ScrollBars.Vertical, BackColor = Color.FromArgb(30, 30, 30),
            ForeColor = Color.FromArgb(200, 200, 200), Font = new Font("Consolas", 8.5f)
        };
        _lblStatus = new Label
        {
            Dock = DockStyle.Bottom, Height = 20, ForeColor = Color.DimGray, Text = ""
        };
        grpLog.Controls.Add(_logBox);
        grpLog.Controls.Add(_lblStatus);

        // ── 폼에 추가 (역순 — DockStyle.Top은 위에서부터 쌓임) ──────────────
        Controls.Add(grpLog);
        Controls.Add(pnlAction);
        Controls.Add(grpOutput);
        Controls.Add(grpFormat);
        Controls.Add(grpExcel);

        // ── 기본 경로 설정 ───────────────────────────────────────────────────
        string exeDir = AppContext.BaseDirectory;
        string defaultExcel  = Path.GetFullPath(Path.Combine(exeDir, @"..\..\..\..\tables\excel"));
        string defaultOutput = Path.GetFullPath(Path.Combine(exeDir, @"..\..\..\..\tables\out"));
        _excelFolderBox.Text  = Directory.Exists(defaultExcel)  ? defaultExcel  : exeDir;
        _outputFolderBox.Text = Directory.Exists(defaultOutput) ? defaultOutput : Path.Combine(exeDir, "out");

        // ── 이벤트 연결 ──────────────────────────────────────────────────────
        btnBrowseExcel.Click += (_, _) => BrowseFolder(_excelFolderBox, "Excel 폴더 선택");
        btnBrowseOut.Click   += (_, _) => BrowseFolder(_outputFolderBox, "출력 폴더 선택");
        btnRefresh.Click     += (_, _) => RefreshFileList();
        btnSelectAll.Click   += (_, _) => SetAllChecked(true);
        btnDeselectAll.Click += (_, _) => SetAllChecked(false);
        _btnExport.Click     += (_, _) => RunExport();
        _chkSql.CheckedChanged += (_, _) => UpdateSqlOptionsVisible();
        pnlAction.SizeChanged += (_, _) => _btnExport.Location = new Point(pnlAction.Width - 132, 5);

        UpdateSqlOptionsVisible();
        RefreshFileList();
    }

    // ── 파일 목록 새로고침 ───────────────────────────────────────────────────

    private void RefreshFileList()
    {
        _fileList.Items.Clear();
        string folder = _excelFolderBox.Text.Trim();
        if (!Directory.Exists(folder))
        {
            Log($"[경고] 폴더 없음: {folder}");
            return;
        }

        var files = Directory.GetFiles(folder, "*.xlsx")
                             .OrderBy(f => f)
                             .Select(Path.GetFileName)
                             .Where(n => n is not null)
                             .Cast<string>()
                             .ToArray();

        foreach (var name in files)
            _fileList.Items.Add(name, true);

        _lblStatus.Text = $"파일 {files.Length}개 발견";
    }

    // ── 내보내기 실행 ────────────────────────────────────────────────────────

    private void RunExport()
    {
        var selectedFiles = _fileList.CheckedItems.Cast<string>().ToList();
        if (selectedFiles.Count == 0)
        {
            MessageBox.Show("내보낼 Excel 파일을 하나 이상 선택하세요.", "알림",
                MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        var formats = new List<string>();
        if (_chkCsv.Checked)  formats.Add("csv");
        if (_chkJson.Checked) formats.Add("json");
        if (_chkSql.Checked)  formats.Add("sql");

        if (formats.Count == 0)
        {
            MessageBox.Show("출력 형식을 하나 이상 선택하세요.", "알림",
                MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        string excelFolder = _excelFolderBox.Text.Trim();
        string outputBase  = _outputFolderBox.Text.Trim();
        bool   subFolders  = _chkSubFolders.Checked;

        SqlDialect dialect = _dialectCombo.SelectedIndex switch
        {
            1 => SqlDialect.MSSQL,
            2 => SqlDialect.SQLite,
            3 => SqlDialect.PostgreSQL,
            _ => SqlDialect.MySQL
        };
        bool schema = _chkSchema.Checked;

        _logBox.Clear();
        _btnExport.Enabled = false;
        _lblStatus.Text = "변환 중...";

        // 백그라운드에서 실행 (UI 블로킹 방지)
        Task.Run(() =>
        {
            int totalTables = 0;
            int errors = 0;

            foreach (string fileName in selectedFiles)
            {
                string filePath = Path.Combine(excelFolder, fileName);
                AppendLog($"읽는 중: {fileName}");

                List<TableData> tables;
                try
                {
                    tables = ExcelReader.Read(filePath);
                }
                catch (Exception ex)
                {
                    AppendLog($"  [오류] {ex.Message}");
                    errors++;
                    continue;
                }

                foreach (TableData table in tables)
                {
                    foreach (string fmt in formats)
                    {
                        string outDir = (subFolders && formats.Count > 1)
                            ? Path.Combine(outputBase, fmt)
                            : (subFolders ? Path.Combine(outputBase, fmt) : outputBase);

                        try
                        {
                            Directory.CreateDirectory(outDir);
                            IExporter exporter = fmt switch
                            {
                                "json" => new JsonExporter(),
                                "sql"  => new SqlExporter(dialect, schema),
                                _      => new CsvExporter()
                            };
                            exporter.Export(table, outDir);
                        }
                        catch (Exception ex)
                        {
                            AppendLog($"  [오류] {table.TableName} ({fmt}): {ex.Message}");
                            errors++;
                        }
                    }
                    totalTables++;
                }
            }

            string summary = errors == 0
                ? $"완료. {totalTables}개 테이블 → [{outputBase}]"
                : $"완료 (오류 {errors}건). {totalTables}개 테이블 → [{outputBase}]";

            AppendLog("");
            AppendLog(summary);

            Invoke(() =>
            {
                _btnExport.Enabled = true;
                _lblStatus.Text = summary;
            });
        });
    }

    // ── 헬퍼 ─────────────────────────────────────────────────────────────────

    private void UpdateSqlOptionsVisible()
    {
        bool sqlOn = _chkSql.Checked;
        _dialectCombo.Enabled = sqlOn;
        _chkSchema.Enabled    = sqlOn;
    }

    private void SetAllChecked(bool value)
    {
        for (int i = 0; i < _fileList.Items.Count; i++)
            _fileList.SetItemChecked(i, value);
    }

    private void BrowseFolder(TextBox target, string description)
    {
        using var dlg = new FolderBrowserDialog { Description = description };
        if (Directory.Exists(target.Text))
            dlg.InitialDirectory = target.Text;
        if (dlg.ShowDialog(this) == DialogResult.OK)
            target.Text = dlg.SelectedPath;
    }

    private void Log(string message) =>
        Invoke(() => _logBox.AppendText(message + Environment.NewLine));

    private void AppendLog(string message)
    {
        if (InvokeRequired)
            Invoke(() => _logBox.AppendText(message + Environment.NewLine));
        else
            _logBox.AppendText(message + Environment.NewLine);
    }

    private static Button MakeButton(string text, int width) =>
        new() { Text = text, Width = width, Height = 26, Margin = new Padding(2, 3, 2, 0) };
}
