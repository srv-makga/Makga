using System.Globalization;
using DeployTool.Manager.Components;
using DeployTool.Manager.Services;
using Microsoft.AspNetCore.Localization;

var builder = WebApplication.CreateBuilder(args);

// ─── 서비스 등록 ──────────────────────────────

builder.Services.Configure<ManagerConfig>(builder.Configuration.GetSection("DeployManager"));
builder.Services.AddSingleton<AgentRegistry>();
builder.Services.AddHostedService<ServerListService>();
builder.Services.AddLocalization(opts => opts.ResourcesPath = "Resources");
builder.Services.AddRazorComponents()
	.AddInteractiveServerComponents();

// ─── 미들웨어 파이프라인 구성 ─────────────────

var app = builder.Build();

if (!app.Environment.IsDevelopment())
	app.UseHsts();

app.UseStaticFiles();

var supportedCultures = new[] { new CultureInfo("ko"), new CultureInfo("en") };
app.UseRequestLocalization(new RequestLocalizationOptions
{
	DefaultRequestCulture   = new RequestCulture("ko"),
	SupportedCultures       = supportedCultures,
	SupportedUICultures     = supportedCultures,
	RequestCultureProviders = new List<IRequestCultureProvider>
	{
		new QueryStringRequestCultureProvider(),
		new CookieRequestCultureProvider(),
		new AcceptLanguageHeaderRequestCultureProvider(),
	}
});

app.UseAntiforgery();

app.MapRazorComponents<App>()
	.AddInteractiveServerRenderMode();

app.UseWebSockets();

app.Run();