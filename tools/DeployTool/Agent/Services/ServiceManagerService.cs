using System.Runtime.InteropServices;
using System.ServiceProcess;
using DeployTool.Protocol.Packets;

namespace DeployTool.Agent.Services;

public class ServiceManagerService
{
	public async Task<ServiceStatusResponse> StartAsync(ServiceNameRequest req)
	{
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			var sc = new ServiceController(req.Name);
			if (sc.Status != ServiceControllerStatus.Running)
			{
				sc.Start();
				await Task.Run(() => sc.WaitForStatus(ServiceControllerStatus.Running, TimeSpan.FromSeconds(30)));
			}
			sc.Refresh();
			return new ServiceStatusResponse { Name = req.Name, Status = sc.Status.ToString() };
		}
		else
		{
			await RunSystemctlAsync("start", req.Name);
			return new ServiceStatusResponse { Name = req.Name, Status = await GetSystemctlStatusAsync(req.Name) };
		}
	}

	public async Task<ServiceStatusResponse> StopAsync(ServiceNameRequest req)
	{
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			var sc = new ServiceController(req.Name);
			if (sc.Status != ServiceControllerStatus.Stopped)
			{
				sc.Stop();
				await Task.Run(() => sc.WaitForStatus(ServiceControllerStatus.Stopped, TimeSpan.FromSeconds(30)));
			}
			sc.Refresh();
			return new ServiceStatusResponse { Name = req.Name, Status = sc.Status.ToString() };
		}
		else
		{
			await RunSystemctlAsync("stop", req.Name);
			return new ServiceStatusResponse { Name = req.Name, Status = await GetSystemctlStatusAsync(req.Name) };
		}
	}

	public async Task<ServiceStatusResponse> GetStatusAsync(ServiceNameRequest req)
	{
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			var sc = new ServiceController(req.Name);
			return new ServiceStatusResponse { Name = req.Name, Status = sc.Status.ToString() };
		}
		return new ServiceStatusResponse { Name = req.Name, Status = await GetSystemctlStatusAsync(req.Name) };
	}

	public Task<ServiceListResponse> ListAsync()
	{
		var list = new List<ServiceEntry>();
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			foreach (var sc in ServiceController.GetServices())
				list.Add(new ServiceEntry
				{
					Name        = sc.ServiceName,
					DisplayName = sc.DisplayName,
					Status      = sc.Status.ToString()
				});
		}
		return Task.FromResult(new ServiceListResponse { Services = list });
	}

	private static async Task RunSystemctlAsync(string command, string name)
	{
		using var proc = new System.Diagnostics.Process();
		proc.StartInfo = new System.Diagnostics.ProcessStartInfo
		{
			FileName               = "systemctl",
			ArgumentList           = { command, name },
			RedirectStandardOutput = true,
			RedirectStandardError  = true,
			UseShellExecute        = false
		};
		proc.Start();
		await proc.WaitForExitAsync();
	}

	private static async Task<string> GetSystemctlStatusAsync(string name)
	{
		using var proc = new System.Diagnostics.Process();
		proc.StartInfo = new System.Diagnostics.ProcessStartInfo
		{
			FileName               = "systemctl",
			ArgumentList           = { "is-active", name },
			RedirectStandardOutput = true,
			UseShellExecute        = false
		};
		proc.Start();
		var result = await proc.StandardOutput.ReadToEndAsync();
		return result.Trim();
	}
}
