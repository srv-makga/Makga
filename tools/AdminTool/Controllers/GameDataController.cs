using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Localization;
using AdminTool.Models;
using AdminTool.Resources;
using AdminTool.Services;

namespace AdminTool.Controllers;

[Authorize]
public class GameDataController : Controller
{
    private readonly DbService _db;
    private readonly IStringLocalizer<SharedResource> _locale;
    public GameDataController(DbService db, IStringLocalizer<SharedResource> l)
    { _db = db; _locale = l; }

    public async Task<IActionResult> Index()
    {
        var tables = await _db.GetTablesAsync();
        var first = tables.FirstOrDefault(t => !t.StartsWith("admin_")) ?? tables.FirstOrDefault();
        if (first == null) { ViewData["Title"] = _locale["GameData_Title"].Value; return View("Empty"); }
        return RedirectToAction("Table", new { table = first });
    }

    public async Task<IActionResult> Table(string table, string? search, int page = 1, int pageSize = 50)
    {
        var allTables = await _db.GetTablesAsync();
        if (!allTables.Contains(table, StringComparer.OrdinalIgnoreCase))
            return RedirectToAction("Index");

        var (columns, rows, total) = await _db.GetTableDataAsync(table, search, page, pageSize);
        var vm = new TableViewModel
        {
            TableName = table,
            Columns = columns,
            Rows = rows,
            Total = total,
            Page = page,
            PageSize = pageSize,
            Search = search,
            AllTables = allTables,
        };

        ViewData["Title"] = $"{_locale["GameData_Title"].Value} - {table}";
        return View(vm);
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin,admin")]
    public async Task<IActionResult> UpdateRow([FromForm] string table, [FromForm] IFormCollection form)
    {
        try
        {
            var cols = await _db.GetColumnsAsync(table);
            var pkCol = cols.FirstOrDefault(c => c.IsPrimaryKey)
                ?? throw new Exception(_locale["GameData_NoPrimaryKey"].Value);

            var values = cols
                .Where(c => form.ContainsKey(c.Name))
                .ToDictionary(c => c.Name, c => form[c.Name].ToString() is "" ? null : form[c.Name].ToString());

            if (!values.TryGetValue(pkCol.Name, out var pkValue) || pkValue == null)
                throw new Exception(_locale["GameData_PkValueMissing"].Value);

            await _db.UpdateRowAsync(table, values, pkCol.Name, pkValue);
            return Json(new { success = true });
        }
        catch (Exception ex) { return Json(new { success = false, error = ex.Message }); }
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin,admin")]
    public async Task<IActionResult> InsertRow([FromForm] string table, [FromForm] IFormCollection form)
    {
        try
        {
            var cols = await _db.GetColumnsAsync(table);
            var values = cols
                .Where(c => !c.IsAutoIncrement && form.ContainsKey(c.Name))
                .ToDictionary(c => c.Name, c => form[c.Name].ToString() is "" ? null : form[c.Name].ToString());

            await _db.InsertRowAsync(table, values);
            return Json(new { success = true });
        }
        catch (Exception ex) { return Json(new { success = false, error = ex.Message }); }
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin")]
    public async Task<IActionResult> DeleteRow([FromForm] string table,
        [FromForm] string pkCol, [FromForm] string pkValue)
    {
        try
        {
            await _db.DeleteRowAsync(table, pkCol, pkValue);
            TempData["Success"] = _locale["GameData_RowDeleted"].Value;
        }
        catch (Exception ex) { TempData["Error"] = ex.Message; }
        return RedirectToAction("Table", new { table });
    }
}
