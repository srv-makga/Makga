using System.Net;
using System.Net.NetworkInformation;
using System.Runtime.InteropServices;
using DeployTool.Protocol.Packets;
using Microsoft.Extensions.Options;

namespace DeployTool.Agent.Services;

public class ResourceService
{
	private readonly AgentConfig _cfg;
	private readonly DateTime    _startTime = DateTime.UtcNow;

	public ResourceService(IOptions<AgentConfig> cfg) => _cfg = cfg.Value;

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

	public Task<PortStatusResponse> CheckPortAsync(CheckPortRequest req)
	{
		var props     = IPGlobalProperties.GetIPGlobalProperties();
		var listeners = props.GetActiveTcpListeners();
		var listening = listeners.Any(ep => ep.Port == req.Port);
		return Task.FromResult(new PortStatusResponse { Port = req.Port, Listening = listening });
	}

	private static long ParseMemInfoKb(string line)
	{
		var parts = line.Split(':', StringSplitOptions.TrimEntries);
		return long.TryParse(parts[1].Split(' ')[0], out var v) ? v : 0;
	}
}
