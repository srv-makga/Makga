using System.Net;
using System.Net.NetworkInformation;
using System.Runtime.InteropServices;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Options;

namespace DeployTool.Agent.Services;

/// <summary>
/// 메모리, 디스크, 네트워크 및 포트 상태를 포함한 시스템 리소스 정보를 제공합니다.
/// </summary>
public class ResourceService
{
	private readonly AgentConfig _cfg;
	private readonly DateTime    _startTime = DateTime.UtcNow;
	private readonly ExternalServiceMonitor _serviceMonitor;

	/// <summary>
	/// ResourceService 클래스의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="cfg">에이전트 설정 옵션</param>
	public ResourceService(IOptions<AgentConfig> cfg)
	{
		_cfg = cfg.Value;
		_serviceMonitor = new ExternalServiceMonitor(_cfg.Services);
	}

	/// <summary>
	/// 버전, OS, 호스트명, 작업 디렉터리 및 가동 시간을 포함한 에이전트 정보를 검색합니다.
	/// </summary>
	/// <returns>에이전트 정보를 포함하는 응답</returns>
	public Task<AgentInfoResponse> GetAgentInfoAsync()
	{
		return Task.FromResult(new AgentInfoResponse
		{
			Version   = "1.0.0",
			Os        = RuntimeInformation.OSDescription,
			Hostname  = Dns.GetHostName(),
			WorkDir   = _cfg.WorkDir,
			UptimeSec = (ulong)(DateTime.UtcNow - _startTime).TotalSeconds
		});
	}

	/// <summary>
	/// 메모리 및 디스크 사용량을 포함한 시스템 리소스 정보를 검색합니다.
	/// </summary>
	/// <returns>리소스 통계를 포함하는 응답</returns>
	public Task<ResourceInfoResponse> GetResourceAsync()
	{
		var info = new ResourceInfoResponse
		{
			Os = RuntimeInformation.OSDescription
		};

		// 메모리
		try
		{
			if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
			{
				var lines = File.ReadAllLines("/proc/meminfo");
				long total = 0, avail = 0;
				foreach (var line in lines)
				{
					if (line.StartsWith("MemTotal:"))
						total = ParseMemInfoKb(line);
					else if (line.StartsWith("MemAvailable:"))
						avail = ParseMemInfoKb(line);
				}
				info.MemTotalMb = total / 1024;
				info.MemUsedMb  = (total - avail) / 1024;
			}
			else
			{
				var gcMem = GC.GetGCMemoryInfo();
				info.MemTotalMb = gcMem.TotalAvailableMemoryBytes / 1024 / 1024;
				info.MemUsedMb  = (gcMem.TotalAvailableMemoryBytes - gcMem.MemoryLoadBytes) / 1024 / 1024;
			}
		}
		catch { }

		// 디스크
		try
		{
			var drive = new DriveInfo(Path.GetPathRoot(_cfg.WorkDir) ?? "/");
			info.DiskTotalGb = drive.TotalSize / 1024.0 / 1024.0 / 1024.0;
			info.DiskUsedGb  = (drive.TotalSize - drive.AvailableFreeSpace) / 1024.0 / 1024.0 / 1024.0;
		}
		catch { }

		return Task.FromResult(info);
	}

	/// <summary>
	/// 송수신한 바이트를 포함한 네트워크 인터페이스 통계를 검색합니다.
	/// </summary>
	/// <returns>네트워크 인터페이스 정보를 포함하는 응답</returns>
	public Task<NetworkInfoResponse> GetNetworkAsync()
	{
		var ifaces = new List<NetworkInterfaceEntry>();
		foreach (var nic in NetworkInterface.GetAllNetworkInterfaces())
		{
			var stats = nic.GetIPStatistics();
			ifaces.Add(new NetworkInterfaceEntry
			{
				Name      = nic.Name,
				BytesSent = stats.BytesSent,
				BytesRecv = stats.BytesReceived
			});
		}
		return Task.FromResult(new NetworkInfoResponse { Interfaces = ifaces });
	}

	/// <summary>
	/// 특정 TCP 포트가 수신 대기 중인지 확인합니다.
	/// </summary>
	/// <param name="req">포트 번호를 포함하는 요청</param>
	/// <returns>포트가 수신 대기 중인지 여부를 나타내는 응답</returns>
	public Task<PortStatusResponse> CheckPortAsync(CheckPortRequest req)
	{
		var props     = IPGlobalProperties.GetIPGlobalProperties();
		var listeners = props.GetActiveTcpListeners();
		var listening = listeners.Any(ep => ep.Port == req.Port);
		return Task.FromResult(new PortStatusResponse { Port = req.Port, Listening = listening });
	}

	/// <summary>
	/// 설정된 모든 외부 서비스(MySQL, MSSQL, Redis 등)의 상태를 검색합니다.
	/// </summary>
	/// <returns>외부 서비스 상태 목록을 포함하는 응답</returns>
	public async Task<ExternalServicesInfoResponse> GetExternalServicesAsync()
	{
		var services = await _serviceMonitor.GetAllServicesAsync();
		return new ExternalServicesInfoResponse
		{
			Services = services,
			CollectedAtUtc = DateTime.UtcNow
		};
	}

	private static long ParseMemInfoKb(string line)
	{
		var parts = line.Split(':', StringSplitOptions.TrimEntries);
		return long.TryParse(parts[1].Split(' ')[0], out var v) ? v : 0;
	}
}
