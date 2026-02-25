using DeployTool.Protocol;
using DeployTool.Protocol.Net;
using DeployTool.Protocol.Packets;
using Microsoft.Extensions.Logging;

namespace DeployTool.Agent.Services;

public class CommandDispatcher
{
	private readonly FileService           _file;
	private readonly ProcessService        _process;
	private readonly ResourceService       _resource;
	private readonly ServiceManagerService _svcMgr;
	private readonly ShellService          _shell;
	private readonly ConfigService         _config;
	private readonly ILogger<CommandDispatcher> _log;

	public CommandDispatcher(
		FileService file, ProcessService process, ResourceService resource,
		ServiceManagerService svcMgr, ShellService shell, ConfigService config,
		ILogger<CommandDispatcher> log)
	{
		_file     = file;
		_process  = process;
		_resource = resource;
		_svcMgr   = svcMgr;
		_shell    = shell;
		_config   = config;
		_log      = log;
	}

	// @brief 패킷 ID에 따라 적절한 서비스로 라우팅
	public async Task<byte[]?> DispatchAsync(PacketId id, byte[] payload, CancellationToken ct)
	{
		try
		{
			return id switch
			{
				PacketId.GetInfo      => Serialize(PacketId.AgentInfo,    await _resource.GetAgentInfoAsync()),
				PacketId.SetWorkDir   => Serialize(PacketId.Result,       await _file.SetWorkDirAsync(Des<SetWorkDirRequest>(payload))),
				PacketId.ListFiles    => Serialize(PacketId.FileList,     await _file.ListFilesAsync(Des<ListFilesRequest>(payload))),
				PacketId.GetFile      => Serialize(PacketId.FileData,     await _file.GetFileAsync(Des<GetFileRequest>(payload))),
				PacketId.PutFile      => Serialize(PacketId.Result,       await _file.PutFileAsync(Des<PutFileRequest>(payload))),
				PacketId.DeleteFile   => Serialize(PacketId.Result,       await _file.DeleteFileAsync(Des<DeleteFileRequest>(payload))),
				PacketId.MoveFile     => Serialize(PacketId.Result,       await _file.MoveFileAsync(Des<MoveFileRequest>(payload))),
				PacketId.MakeDir      => Serialize(PacketId.Result,       await _file.MakeDirAsync(Des<MakeDirRequest>(payload))),
				PacketId.GetFileHash  => Serialize(PacketId.FileHash,     await _file.GetFileHashAsync(Des<GetFileHashRequest>(payload))),
				PacketId.SyncDir      => Serialize(PacketId.SyncDiff,     await _file.SyncDirAsync(Des<SyncDirRequest>(payload))),
				PacketId.BackupDir    => Serialize(PacketId.Result,       await _file.BackupDirAsync(Des<BackupDirRequest>(payload))),
				PacketId.Compress     => Serialize(PacketId.Result,       await _file.CompressAsync(Des<CompressRequest>(payload))),
				PacketId.Decompress   => Serialize(PacketId.Result,       await _file.DecompressAsync(Des<DecompressRequest>(payload))),
				PacketId.Execute      => Serialize(PacketId.Result,       await _process.ExecuteAsync(Des<ExecuteRequest>(payload))),
				PacketId.Kill         => Serialize(PacketId.Result,       await _process.KillAsync(Des<KillRequest>(payload))),
				PacketId.ListProcess  => Serialize(PacketId.ProcessList,  await _process.ListProcessAsync()),
				PacketId.ServiceStart  => Serialize(PacketId.ServiceResult, await _svcMgr.StartAsync(Des<ServiceNameRequest>(payload))),
				PacketId.ServiceStop   => Serialize(PacketId.ServiceResult, await _svcMgr.StopAsync(Des<ServiceNameRequest>(payload))),
				PacketId.ServiceStatus => Serialize(PacketId.ServiceResult, await _svcMgr.GetStatusAsync(Des<ServiceNameRequest>(payload))),
				PacketId.ServiceList   => Serialize(PacketId.ServiceItems,  await _svcMgr.ListAsync()),
				PacketId.GetResource  => Serialize(PacketId.ResourceInfo, await _resource.GetResourceAsync()),
				PacketId.GetNetwork   => Serialize(PacketId.NetworkInfo,  await _resource.GetNetworkAsync()),
				PacketId.CheckPort    => Serialize(PacketId.PortStatus,   await _resource.CheckPortAsync(Des<CheckPortRequest>(payload))),
				PacketId.GetLog       => Serialize(PacketId.LogData,      await _shell.GetLogAsync(Des<GetLogRequest>(payload))),
				PacketId.Shell        => Serialize(PacketId.ShellOutput,  await _shell.RunAsync(Des<ShellRequest>(payload))),
				PacketId.GetConfig    => Serialize(PacketId.ConfigData,   await _config.GetConfigAsync()),
				PacketId.SetConfig    => Serialize(PacketId.Result,       await _config.SetConfigAsync(Des<SetConfigRequest>(payload))),
				PacketId.ReloadConfig => Serialize(PacketId.Result,       await _config.ReloadConfigAsync()),
				_                     => null
			};
		}
		catch (Exception ex)
		{
			_log.LogError(ex, "Dispatch error for {Id}", id);
			return Serialize(PacketId.Result, new ResultResponse
			{
				Success = false,
				Message = ex.Message,
				Code    = 500
			});
		}
	}

	private static byte[] Serialize<T>(PacketId id, T data) =>
		PacketSerializer.Serialize(id, data);

	private static T Des<T>(byte[] payload) =>
		PacketSerializer.Deserialize<T>(payload)
		?? throw new InvalidOperationException($"Failed to deserialize {typeof(T).Name}");
}
