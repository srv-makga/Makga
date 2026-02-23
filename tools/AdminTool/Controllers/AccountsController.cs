using AdminTool.Models;
using AdminTool.Resources;
using AdminTool.Services;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Localization;
using System.Security.Claims;

namespace AdminTool.Controllers;

[Authorize]
public class AccountsController : Controller
{
    private readonly DbService _db;
    private readonly IStringLocalizer<SharedResource> _locale;

    public AccountsController(DbService db, IStringLocalizer<SharedResource> l)
    {
        _db = db;
        _locale = l;
    }

    [Authorize(Roles = "superadmin")]
    public async Task<IActionResult> Index()
    {
        var accounts = await _db.QueryAsync<AdminAccount>("SELECT * FROM admin_accounts ORDER BY id");
        ViewData["Title"] = _locale["Account_Title"].Value;
        return View(accounts.ToList());
    }

    [Authorize(Roles = "superadmin")]
    public IActionResult Create()
    {
        ViewData["Title"] = _locale["Account_CreateTitle"].Value;
        return View(new CreateAccountViewModel());
    }

    [HttpPost, Authorize(Roles = "superadmin"), ValidateAntiForgeryToken]
    public async Task<IActionResult> Create(CreateAccountViewModel vm)
    {
        if (vm.Password != vm.ConfirmPassword)
        { TempData["Error"] = _locale["Account_PasswordMismatch"].Value; return View(vm); }

        var exists = await _db.QuerySingleAsync<int>(
            "SELECT COUNT(*) FROM admin_accounts WHERE username = @u", new { u = vm.Username });
        if (exists > 0)
        { TempData["Error"] = _locale["Account_AlreadyExists"].Value; return View(vm); }

        var hash = BCrypt.Net.BCrypt.HashPassword(vm.Password, 12);
        await _db.ExecuteAsync(
            "INSERT INTO admin_accounts (username, password_hash, role) VALUES (@u, @h, @r)",
            new { u = vm.Username, h = hash, r = vm.Role });

        TempData["Success"] = string.Format(_locale["Account_Created"].Value, vm.Username);
        return RedirectToAction("Index");
    }

    [Authorize(Roles = "superadmin")]
    public async Task<IActionResult> Edit(int id)
    {
        var account = await _db.QuerySingleAsync<AdminAccount>("SELECT * FROM admin_accounts WHERE id = @id", new { id });
        if (account == null)
        {
            return NotFound();
        }

        ViewData["Title"] = _locale["Account_EditTitle"].Value;

        return View(new EditAccountViewModel
        {
            Id = account.Id,
            Username = account.Username,
            Role = account.Role,
            IsActive = account.IsActive,
        });
    }

    [HttpPost, Authorize(Roles = "superadmin"), ValidateAntiForgeryToken]
    public async Task<IActionResult> Edit(EditAccountViewModel vm)
    {
        var currentUserId = int.Parse(User.FindFirstValue(ClaimTypes.NameIdentifier)!);
        if (vm.Id == currentUserId && !vm.IsActive)
        {
            TempData["Error"] = _locale["Account_CannotDeactivateSelf"].Value;
            return RedirectToAction("Edit", new { vm.Id });
        }

        await _db.ExecuteAsync(
            "UPDATE admin_accounts SET role = @r, is_active = @a WHERE id = @id",
            new { r = vm.Role, a = vm.IsActive, id = vm.Id });

        if (!string.IsNullOrWhiteSpace(vm.NewPassword))
        {
            if (vm.NewPassword != vm.ConfirmPassword)
            {
                TempData["Error"] = _locale["Account_NewPasswordMismatch"].Value;
                return RedirectToAction("Edit", new { vm.Id });
            }

            var hash = BCrypt.Net.BCrypt.HashPassword(vm.NewPassword, 12);
            await _db.ExecuteAsync("UPDATE admin_accounts SET password_hash = @h WHERE id = @id",
                new { h = hash, id = vm.Id });
        }

        TempData["Success"] = _locale["Account_Updated"].Value;
        return RedirectToAction("Index");
    }

    [HttpPost, Authorize(Roles = "superadmin"), ValidateAntiForgeryToken]
    public async Task<IActionResult> Delete(int id)
    {
        var currentUserId = int.Parse(User.FindFirstValue(ClaimTypes.NameIdentifier)!);
        if (id == currentUserId)
        {
            TempData["Error"] = _locale["Account_CannotDeleteSelf"].Value;
            return RedirectToAction("Index");
        }

        await _db.ExecuteAsync("DELETE FROM admin_accounts WHERE id = @id", new { id });
        TempData["Success"] = _locale["Account_Deleted"].Value;
        return RedirectToAction("Index");
    }

    // Any logged-in user can change their own password
    public IActionResult ChangePassword()
    {
        ViewData["Title"] = _locale["Nav_ChangePassword"].Value;
        return View();
    }

    [HttpPost, ValidateAntiForgeryToken]
    public async Task<IActionResult> ChangePassword(string currentPassword, string newPassword, string confirmPassword)
    {
        if (newPassword != confirmPassword)
        {
            TempData["Error"] = _locale["Account_NewPasswordMismatch"].Value;
            return View();
        }

        var userId = int.Parse(User.FindFirstValue(ClaimTypes.NameIdentifier)!);
        var account = await _db.QuerySingleAsync<AdminAccount>(
            "SELECT * FROM admin_accounts WHERE id = @id", new { id = userId });

        if (account == null || !BCrypt.Net.BCrypt.Verify(currentPassword, account.PasswordHash))
        { TempData["Error"] = _locale["Account_CurrentPasswordWrong"].Value; return View(); }

        var hash = BCrypt.Net.BCrypt.HashPassword(newPassword, 12);
        await _db.ExecuteAsync($"UPDATE admin_accounts SET password_hash = @h WHERE id = @id",
            new { h = hash, id = userId });

        TempData["Success"] = _locale["Account_PasswordChanged"].Value;
        return RedirectToAction("Index", "Dashboard");
    }
}