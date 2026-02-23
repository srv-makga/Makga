using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Localization;
using AdminTool.Models;
using AdminTool.Resources;
using AdminTool.Services;

namespace AdminTool.Controllers;

[Authorize]
public class PlayersController : Controller
{
    private static readonly string[] PlayerTablePatterns =
        ["user", "account", "character", "player", "inventory", "currency"];

    private readonly DbService _db;
    private readonly IStringLocalizer<SharedResource> _locale;
    public PlayersController(DbService db, IStringLocalizer<SharedResource> l)
    { _db = db; _locale = l; }

    public async Task<IActionResult> Index(string? query, string? tableName)
    {
        var allTables = await _db.GetTablesAsync();
        var playerTables = allTables
            .Where(t => PlayerTablePatterns.Any(p => t.Contains(p, StringComparison.OrdinalIgnoreCase)))
            .ToList();

        var vm = new PlayerSearchViewModel
        {
            Query = query,
            TableName = tableName ?? playerTables.FirstOrDefault(),
            PlayerTables = playerTables,
        };

        if (!string.IsNullOrWhiteSpace(vm.TableName) && !string.IsNullOrWhiteSpace(query))
        {
            var (cols, rows, total) = await _db.GetTableDataAsync(vm.TableName, query, 1, 100);
            vm.Columns = cols;
            vm.Results = rows;
            vm.Total = total;
        }

        ViewData["Title"] = _locale["Player_Title"].Value;
        return View(vm);
    }

    public async Task<IActionResult> Detail(string id, string? idColumn, string? sourceTable)
    {
        var allTables = await _db.GetTablesAsync();
        var playerTables = allTables
            .Where(t => PlayerTablePatterns.Any(p => t.Contains(p, StringComparison.OrdinalIgnoreCase)))
            .ToList();

        var vm = new PlayerDetailViewModel { PlayerId = id };

        foreach (var tbl in playerTables)
        {
            try
            {
                var (cols, rows, _) = await _db.GetTableDataAsync(tbl, id, 1, 50);
                if (rows.Count > 0)
                    vm.Tables[tbl] = new TableData { Columns = cols, Rows = rows };
            }
            catch { }
        }

        ViewData["Title"] = $"{_locale["Player_DetailTitle"].Value} - {id}";
        return View(vm);
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin,admin")]
    public async Task<IActionResult> GiveItem([FromForm] string table, [FromForm] string userId,
        [FromForm] int itemId, [FromForm] int count)
    {
        try
        {
            await _db.ExecuteAsync(
                $"INSERT INTO `{table}` (user_id, item_id, count) VALUES (@uid, @iid, @cnt) " +
                "ON DUPLICATE KEY UPDATE count = count + @cnt",
                new { uid = userId, iid = itemId, cnt = count });
            return Json(new { success = true });
        }
        catch (Exception ex) { return Json(new { success = false, error = ex.Message }); }
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin,admin")]
    public async Task<IActionResult> SetCurrency([FromForm] string userId,
        [FromForm] string field, [FromForm] long value)
    {
        var validFields = new[] { "gold", "ruby", "mileage", "honor", "guild_point" };
        if (!validFields.Contains(field.ToLower()))
            return Json(new { success = false, error = _locale["Common_InvalidField"].Value });

        try
        {
            foreach (var tbl in new[] { "user_currency", "user_wallet", "currency" })
            {
                try
                {
                    await _db.ExecuteAsync(
                        $"UPDATE `{tbl}` SET `{field}` = @v WHERE user_id = @uid",
                        new { v = value, uid = userId });
                    return Json(new { success = true });
                }
                catch { }
            }
            return Json(new { success = false, error = _locale["Player_CurrencyTableNotFound"].Value });
        }
        catch (Exception ex) { return Json(new { success = false, error = ex.Message }); }
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin,admin")]
    public async Task<IActionResult> Ban([FromForm] string table, [FromForm] string userId,
        [FromForm] string reason)
    {
        try
        {
            await _db.ExecuteAsync(
                $"UPDATE `{table}` SET status = 'banned', ban_reason = @r WHERE id = @uid",
                new { r = reason, uid = userId });
            TempData["Success"] = string.Format(_locale["Player_Banned"].Value, userId);
        }
        catch (Exception ex) { TempData["Error"] = ex.Message; }
        return RedirectToAction("Index");
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin,admin")]
    public async Task<IActionResult> Unban([FromForm] string table, [FromForm] string userId)
    {
        try
        {
            await _db.ExecuteAsync(
                $"UPDATE `{table}` SET status = 'active', ban_reason = NULL WHERE id = @uid",
                new { uid = userId });
            TempData["Success"] = string.Format(_locale["Player_Unbanned"].Value, userId);
        }
        catch (Exception ex) { TempData["Error"] = ex.Message; }
        return RedirectToAction("Index");
    }
}
