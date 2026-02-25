using DeployTool.Agent.Services;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;

var host = Host.CreateDefaultBuilder(args)
	.ConfigureAppConfiguration(cfg =>
	{
		cfg.SetBasePath(AppContext.BaseDirectory);
		cfg.AddJsonFile("appsettings.json", optional: false, reloadOnChange: true);
	})
	.ConfigureServices((ctx, svc) =>
	{
		svc.Configure<AgentConfig>(ctx.Configuration.GetSection("DeployAgent"));
		svc.AddSingleton<FileService>();
		svc.AddSingleton<ProcessService>();
		svc.AddSingleton<ResourceService>();
		svc.AddSingleton<ServiceManagerService>();
		svc.AddSingleton<ShellService>();
		svc.AddSingleton<ConfigService>();
		svc.AddSingleton<CommandDispatcher>();
		svc.AddHostedService<AgentServer>();
	})
	.ConfigureLogging(log =>
	{
		log.AddConsole();
	})
	.Build();

await host.RunAsync();
