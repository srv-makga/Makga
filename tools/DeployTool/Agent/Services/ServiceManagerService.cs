using System.Runtime.InteropServices;
using System.ServiceProcess;
using DeployTool.Common.Packets;

namespace DeployTool.Agent.Services;

/// <summary>
/// Windows 서비스(Windows) 및 systemd 서비스(Linux)를 시작, 중지 및 상태 작업으로 관리합니다.
/// </summary>
public class ServiceManagerService
{
	/// <summary>
	/// Windows 서비스 또는 systemd 서비스를 시작합니다.
	/// </summary>
	/// <param name="req">서비스 이름을 포함하는 요청</param>
	/// <returns>서비스 상태를 포함하는 응답</returns>
	public async Task<ServiceStatusResponse> StartAsync(ServiceNameRequest req)
	{
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			using var sc = new ServiceController(req.Name);
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

	/// <summary>
	/// Windows 서비스 또는 systemd 서비스를 중지합니다.
	/// </summary>
	/// <param name="req">서비스 이름을 포함하는 요청</param>
	/// <returns>서비스 상태를 포함하는 응답</returns>
	public async Task<ServiceStatusResponse> StopAsync(ServiceNameRequest req)
	{
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			using var sc = new ServiceController(req.Name);
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

	/// <summary>
	/// 서비스의 현재 상태를 가져옵니다.
	/// </summary>
	/// <param name="req">서비스 이름을 포함하는 요청</param>
	/// <returns>현재 서비스 상태를 포함하는 응답</returns>
	public async Task<ServiceStatusResponse> GetStatusAsync(ServiceNameRequest req)
	{
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			using var sc = new ServiceController(req.Name);
			return new ServiceStatusResponse { Name = req.Name, Status = sc.Status.ToString() };
		}
		return new ServiceStatusResponse { Name = req.Name, Status = await GetSystemctlStatusAsync(req.Name) };
	}

	/// <summary>
	/// 사용 가능한 모든 Windows 서비스를 나열합니다 (Windows만 해당).
	/// </summary>
	/// <returns>서비스 목록을 포함하는 응답</returns>
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
