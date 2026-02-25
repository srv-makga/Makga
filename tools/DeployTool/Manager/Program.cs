using DeployTool.Manager.Components;
using DeployTool.Manager.Services;

var builder = WebApplication.CreateBuilder(args);

builder.Services.Configure<ManagerConfig>(builder.Configuration.GetSection("DeployManager"));
builder.Services.AddSingleton<AgentRegistry>();
builder.Services.AddHostedService<ServerListService>();
builder.Services.AddRazorComponents()
	.AddInteractiveServerComponents();

var app = builder.Build();

if (!app.Environment.IsDevelopment())
	app.UseHsts();

app.UseStaticFiles();
app.UseAntiforgery();
app.MapRazorComponents<App>()
	.AddInteractiveServerRenderMode();

app.Run();
