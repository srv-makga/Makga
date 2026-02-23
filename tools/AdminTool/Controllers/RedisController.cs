using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AdminTool.Models;
using AdminTool.Services;

namespace AdminTool.Controllers;

[Authorize]
public class RedisController : Controller
{
    private readonly RedisService _redis;
    public RedisController(RedisService redis) => _redis = redis;

    public IActionResult Index(string? pattern)
    {
        ViewData["Title"] = "Redis 브라우저";
        ViewBag.Pattern = pattern ?? "*";
        return View();
    }

    [HttpGet]
    public async Task<IActionResult> Scan(string pattern = "*")
    {
        try
        {
            var keys = await _redis.ScanKeysAsync(pattern, 300);
            return Json(new { success = true, keys });
        }
        catch (Exception ex) { return Json(new { success = false, error = ex.Message }); }
    }

    [HttpGet]
    public async Task<IActionResult> KeyInfo(string key)
    {
        try
        {
            var info = await _redis.GetKeyInfoAsync(key);
            if (info == null) return Json(new { success = false, error = "Key not found" });
            return Json(new { success = true, info });
        }
        catch (Exception ex) { return Json(new { success = false, error = ex.Message }); }
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin,admin")]
    public async Task<IActionResult> SetKey([FromBody] SetKeyRequest req)
    {
        try
        {
            await _redis.SetStringAsync(req.Key, req.Value, req.Ttl > 0 ? req.Ttl : null);
            return Json(new { success = true });
        }
        catch (Exception ex) { return Json(new { success = false, error = ex.Message }); }
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin,admin")]
    public async Task<IActionResult> DeleteKey([FromBody] DeleteKeyRequest req)
    {
        try
        {
            await _redis.DeleteKeyAsync(req.Key);
            return Json(new { success = true });
        }
        catch (Exception ex) { return Json(new { success = false, error = ex.Message }); }
    }

    [HttpGet]
    public async Task<IActionResult> ServerInfo()
    {
        var info = await _redis.GetServerSummaryAsync();
        return Json(new { info });
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin")]
    public async Task<IActionResult> Flush()
    {
        try
        {
            await _redis.FlushDbAsync();
            return Json(new { success = true });
        }
        catch (Exception ex) { return Json(new { success = false, error = ex.Message }); }
    }
}

public record SetKeyRequest(string Key, string Value, long Ttl);
public record DeleteKeyRequest(string Key);
