using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Localization;
using Microsoft.AspNetCore.Mvc;

namespace AdminTool.Controllers;

[Authorize]
public class CultureController : Controller
{
    [HttpPost, ValidateAntiForgeryToken]
    public IActionResult Set(string culture, string returnUrl = "/")
    {
        var supported = new[] { "ko", "en" };
        if (!supported.Contains(culture)) culture = "ko";

        Response.Cookies.Append(
            CookieRequestCultureProvider.DefaultCookieName,
            CookieRequestCultureProvider.MakeCookieValue(new RequestCulture(culture)),
            new CookieOptions { Expires = DateTimeOffset.UtcNow.AddYears(1), IsEssential = true });

        return LocalRedirect(string.IsNullOrEmpty(returnUrl) ? "/" : returnUrl);
    }
}
