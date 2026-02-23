using System.Security.Claims;
using Microsoft.AspNetCore.Authentication;
using Microsoft.AspNetCore.Authentication.Cookies;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Localization;
using AdminTool.Models;
using AdminTool.Resources;
using AdminTool.Services;

namespace AdminTool.Controllers;

public class AuthController : Controller
{
    private readonly DbService _db;
    private readonly IStringLocalizer<SharedResource> _locale;
    public AuthController(DbService db, IStringLocalizer<SharedResource> l)
    { _db = db; _locale = l; }

    [AllowAnonymous]
    public IActionResult Login()
    {
        if (User.Identity?.IsAuthenticated == true)
            return RedirectToAction("Index", "Dashboard");
        return View();
    }

    [HttpPost, AllowAnonymous, ValidateAntiForgeryToken]
    public async Task<IActionResult> Login(string username, string password, string? returnUrl)
    {
        var account = await _db.QuerySingleAsync<AdminAccount>(
            "SELECT * FROM admin_accounts WHERE username = @u AND is_active = 1",
            new { u = username });

        if (account == null || !BCrypt.Net.BCrypt.Verify(password, account.PasswordHash))
        {
            ViewBag.Error = _locale["Login_InvalidCredentials"].Value;
            return View();
        }

        await _db.ExecuteAsync(
            "UPDATE admin_accounts SET last_login = NOW() WHERE id = @id",
            new { id = account.Id });

        var claims = new List<Claim>
        {
            new(ClaimTypes.NameIdentifier, account.Id.ToString()),
            new(ClaimTypes.Name, account.Username),
            new(ClaimTypes.Role, account.Role),
        };
        var identity = new ClaimsIdentity(claims, CookieAuthenticationDefaults.AuthenticationScheme);
        await HttpContext.SignInAsync(
            CookieAuthenticationDefaults.AuthenticationScheme,
            new ClaimsPrincipal(identity),
            new AuthenticationProperties { IsPersistent = true });

        return LocalRedirect(returnUrl ?? "/");
    }

    [Authorize]
    public async Task<IActionResult> Logout()
    {
        await HttpContext.SignOutAsync(CookieAuthenticationDefaults.AuthenticationScheme);
        return RedirectToAction("Login");
    }

    public IActionResult Denied()
    {
        ViewData["Title"] = _locale["Error_AccessDenied"].Value;
        return View();
    }
}
