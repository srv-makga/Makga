using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AdminTool.Models;
using AdminTool.Services;

namespace AdminTool.Controllers;

[Authorize]
public class DashboardController : Controller
{
    private readonly DbService _db;
    private readonly RedisService _redis;
    private readonly ServerClient _serverClient;
    private readonly MakgaConfig _config;

    public DashboardController(DbService db, RedisService redis,
        ServerClient serverClient, MakgaConfig config)
    {
        _db = db; _redis = redis; _serverClient = serverClient; _config = config;
    }

    public async Task<IActionResult> Index()
    {
        var vm = new DashboardViewModel
        {
            DbOk = await _db.PingAsync(),
            RedisOk = await _redis.PingAsync(),
        };

        // Server status (parallel pings)
        var pingTasks = _config.Servers.Select(async srv =>
        {
            var (online, ms) = await _serverClient.PingAsync(srv.Host, srv.Port);
            return new ServerStatusInfo { Name = srv.Name, Host = srv.Host, Port = srv.Port, IsOnline = online, PingMs = ms };
        });
        vm.ServerStatuses = (await Task.WhenAll(pingTasks)).ToList();

        // Table row counts
        foreach (var tbl in new[] { "item", "monster", "skill", "map", "character_class", "quest", "npc", "shop" })
        {
            try
            {
                var cnt = await _db.QuerySingleAsync<long>($"SELECT COUNT(*) FROM `{tbl}`");
                vm.TableCounts[tbl] = cnt;
            }
            catch { /* table may not exist yet */ }
        }

        vm.RedisInfo = await _redis.GetServerSummaryAsync();

        ViewData["Title"] = "대시보드";
        return View(vm);
    }
}
