using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AdminTool.Models;
using AdminTool.Services;

namespace AdminTool.Controllers;

[Authorize]
public class ServersController : Controller
{
    private readonly ServerClient _client;
    private readonly MakgaConfig _config;

    public ServersController(ServerClient client, MakgaConfig config)
    {
        _client = client; _config = config;
    }

    public IActionResult Index()
    {
        ViewData["Title"] = "서버 관리";
        ViewBag.Servers = _config.Servers;
        return View();
    }

    [HttpPost]
    public async Task<IActionResult> Ping([FromBody] PingRequest req)
    {
        var srv = _config.Servers.FirstOrDefault(s => s.Name == req.ServerName);
        if (srv == null) return Json(new { online = false, ms = (long?)null, error = "Server not found" });

        var (online, ms) = await _client.PingAsync(srv.Host, srv.Port);
        return Json(new { online, ms });
    }

    [HttpPost, ValidateAntiForgeryToken, Authorize(Roles = "superadmin,admin")]
    public async Task<IActionResult> Send([FromBody] PacketSendRequest req)
    {
        var srv = _config.Servers.FirstOrDefault(s => s.Name == req.ServerName);
        if (srv == null)
            return Json(new PacketSendResponse { Success = false, Error = "Server not found" });

        byte[] payload = Array.Empty<byte>();
        if (!string.IsNullOrWhiteSpace(req.PayloadHex))
        {
            try { payload = Convert.FromHexString(req.PayloadHex.Replace(" ", "")); }
            catch { return Json(new PacketSendResponse { Success = false, Error = "Invalid hex payload" }); }
        }

        var result = await _client.SendPacketAsync(
            srv.Host, srv.Port, req.PacketId, payload, req.PacketKey);
        return Json(result);
    }
}

public record PingRequest(string ServerName);
