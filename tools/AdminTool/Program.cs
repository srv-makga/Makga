using System.Globalization;
using Microsoft.AspNetCore.Authentication.Cookies;
using Microsoft.AspNetCore.Localization;
using AdminTool.Models;
using AdminTool.Services;

var builder = WebApplication.CreateBuilder(args);

var makgaConfig = builder.Configuration.GetSection("Makga").Get<MakgaConfig>()
    ?? throw new InvalidOperationException("Makga configuration is missing from appsettings.json");
builder.Services.AddSingleton(makgaConfig);

builder.Services.AddScoped<DbService>();
builder.Services.AddSingleton<RedisService>();
builder.Services.AddSingleton<ServerClient>();
builder.Services.AddHttpClient();

builder.Services.AddAuthentication(CookieAuthenticationDefaults.AuthenticationScheme)
    .AddCookie(options =>
    {
        options.LoginPath = "/Auth/Login";
        options.LogoutPath = "/Auth/Logout";
        options.AccessDeniedPath = "/Auth/Denied";
        options.ExpireTimeSpan = TimeSpan.FromHours(8);
        options.SlidingExpiration = true;
        options.Cookie.HttpOnly = true;
        options.Cookie.SameSite = SameSiteMode.Lax;
    });

builder.Services.AddAuthorization();
builder.Services.AddLocalization(opts => opts.ResourcesPath = "Resources");
builder.Services.AddControllersWithViews()
    .AddViewLocalization()
    .AddDataAnnotationsLocalization();

var app = builder.Build();

if (!app.Environment.IsDevelopment())
    app.UseHsts();

app.UseStaticFiles();
app.UseRouting();

// Localization middleware — reads ?culture=en or Accept-Language header, cookie
var supportedCultures = new[] { new CultureInfo("ko"), new CultureInfo("en") };
app.UseRequestLocalization(new RequestLocalizationOptions
{
    DefaultRequestCulture = new RequestCulture("ko"),
    SupportedCultures = supportedCultures,
    SupportedUICultures = supportedCultures,
    RequestCultureProviders = new List<IRequestCultureProvider>
    {
        new QueryStringRequestCultureProvider(),   // ?culture=en
        new CookieRequestCultureProvider(),        // .AspNetCore.Culture cookie
        new AcceptLanguageHeaderRequestCultureProvider(),
    }
});

app.UseAuthentication();
app.UseAuthorization();

app.MapControllerRoute("default", "{controller=Dashboard}/{action=Index}/{id?}");

app.Run();
