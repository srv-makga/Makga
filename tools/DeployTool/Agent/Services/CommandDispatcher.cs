using DeployTool.Common;
using DeployTool.Common.Net;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Logging;

namespace DeployTool.Agent.Services;

/// <summary>
/// 들어오는 패킷 ID를 적절한 서비스 핸들러로 라우팅하고 직렬화된 응답을 반환합니다.
/// </summary>
public class CommandDispatcher
{
	private readonly FileService           _file;
	private readonly ProcessService        _process;
	private readonly ResourceService       _resource;
	private readonly ServiceManagerService _svcMgr;
	private readonly ShellService          _shell;
	private readonly ConfigService         _config;
	private readonly ILogger<CommandDispatcher> _log;

	/// <summary>
	/// 서비스 종속성을 사용하여 CommandDispatcher 클래스의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="file">파일 작업을 위한 파일 서비스</param>
	/// <param name="process">프로세스 실행 및 관리를 위한 프로세스 서비스</param>
	/// <param name="resource">시스템 리소스 정보를 위한 리소스 서비스</param>
	/// <param name="svcMgr">Windows 서비스 제어를 위한 서비스 매니저</param>
	/// <param name="shell">명령 실행을 위한 셸 서비스</param>
	/// <param name="config">설정 관리를 위한 설정 서비스</param>
	/// <param name="log">로거 인스턴스</param>
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

	/// <summary>
	/// 패킷을 적절한 서비스 핸들러로 디스패치하고 직렬화된 응답을 반환합니다.
	/// </summary>
	/// <param name="id">명령 유형을 나타내는 패킷 ID</param>
	/// <param name="payload">직렬화된 패킷 페이로드</param>
	/// <param name="ct">취소 토큰</param>
	/// <returns>직렬화된 응답 바이트 또는 응답이 생성되지 않은 경우 null</returns>
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
				PacketId.UploadChunk  => Serialize(PacketId.Result,       await _file.UploadChunkAsync(Des<UploadChunkRequest>(payload))),
				PacketId.Execute      => Serialize(PacketId.Result,       await _process.ExecuteAsync(Des<ExecuteRequest>(payload))),
				PacketId.Kill         => Serialize(PacketId.Result,       await _process.KillAsync(Des<KillRequest>(payload))),
				PacketId.ListProcess  => Serialize(PacketId.ProcessList,  await _process.ListProcessAsync()),
				PacketId.GetMonitoredProcesses => Serialize(PacketId.MonitoredProcesses, await _process.GetMonitoredProcessesAsync()),
				PacketId.UpsertMonitoredProcess => Serialize(PacketId.Result, await _process.UpsertMonitoredProcessAsync(Des<UpsertMonitoredProcessRequest>(payload))),
				PacketId.DeleteMonitoredProcess => Serialize(PacketId.Result, await _process.DeleteMonitoredProcessAsync(Des<DeleteMonitoredProcessRequest>(payload))),
				PacketId.RestartMonitoredProcess => Serialize(PacketId.Result, await _process.RestartMonitoredProcessAsync(Des<RestartMonitoredProcessRequest>(payload))),
				PacketId.StartMonitoredProcess => Serialize(PacketId.Result, await _process.StartMonitoredProcessAsync(Des<StartMonitoredProcessRequest>(payload))),
				PacketId.StopMonitoredProcess => Serialize(PacketId.Result, await _process.StopMonitoredProcessAsync(Des<StopMonitoredProcessRequest>(payload))),
				PacketId.ServiceStart  => Serialize(PacketId.ServiceResult, await _svcMgr.StartAsync(Des<ServiceNameRequest>(payload))),
				PacketId.ServiceStop   => Serialize(PacketId.ServiceResult, await _svcMgr.StopAsync(Des<ServiceNameRequest>(payload))),
				PacketId.ServiceStatus => Serialize(PacketId.ServiceResult, await _svcMgr.GetStatusAsync(Des<ServiceNameRequest>(payload))),
				PacketId.ServiceList   => Serialize(PacketId.ServiceItems,  await _svcMgr.ListAsync()),
				PacketId.GetResource  => Serialize(PacketId.ResourceInfo, await _resource.GetResourceAsync()),
				PacketId.GetNetwork   => Serialize(PacketId.NetworkInfo,  await _resource.GetNetworkAsync()),
				PacketId.CheckPort    => Serialize(PacketId.PortStatus,   await _resource.CheckPortAsync(Des<CheckPortRequest>(payload))),
				PacketId.GetExternalServices => Serialize(PacketId.ExternalServicesInfo, await _resource.GetExternalServicesAsync()),
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
