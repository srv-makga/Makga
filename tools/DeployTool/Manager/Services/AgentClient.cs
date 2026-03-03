using System.Net.Sockets;
using DeployTool.Common;
using DeployTool.Common.Models;
using DeployTool.Common.Net;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Options;

namespace DeployTool.Manager.Services;

/// <summary>
/// 단일 에이전트와의 영구 TCP 연결을 관리합니다.
/// 인증, 하트비트 모니터링 및 패킷 기반 RPC 통신을 처리합니다.
/// 연결 손실 시 자동으로 다시 연결을 시도합니다.
/// </summary>
public class AgentClient : IAsyncDisposable
{
	/// <summary>서버 설정 정보</summary>
	public ServerEntry Info { get; }
	/// <summary>현재 에이전트에 연결되고 인증되었으면 true</summary>
	public bool IsConnected { get; private set; }
	/// <summary>하트비트에서 수신한 마지막 리소스 정보</summary>
	public ResourceInfoResponse? LastResource { get; private set; }
	/// <summary>마지막 성공한 하트비트의 타임스탬프</summary>
	public DateTime? LastHeartbeatAt { get; private set; }

	/// <summary>연결 상태 변경 시 발생</summary>
	public event Action? OnStateChanged;

	private readonly IOptionsMonitor<ManagerConfig> _options;
	private int RetryIntervalSec => _options.CurrentValue.RetryIntervalSec;
	private int HeartbeatIntervalSec => _options.CurrentValue.HeartbeatIntervalSec;

	private TcpClient? _tcp;
	private NetworkStream? _stream;

	private readonly SemaphoreSlim _lock = new(1, 1);
	private readonly CancellationTokenSource _cts = new();
	private readonly Task _runLoop;

	/// <summary>
	/// AgentClient 클래스의 새 인스턴스를 초기화합니다.
	/// 연결 및 하트비트 백그라운드 루프를 즉시 시작합니다.
	/// </summary>
	/// <param name="info">서버 항목 설정</param>
	/// <param name="options">재시도/하트비트 간격을 위한 매니저 설정 옵션</param>
	public AgentClient(ServerEntry info, IOptionsMonitor<ManagerConfig> options)
	{
		Info = info;
		_options = options;
		_runLoop = RunLoopAsync(_cts.Token);
	}

	private async Task RunLoopAsync(CancellationToken ct)
	{
		while (!ct.IsCancellationRequested)
		{
			try
			{
				await EstablishAsync(ct);
				await HeartbeatLoopAsync(ct);
			}
			catch (OperationCanceledException)
			{
				return;
			}
			catch
			{
				SetDisconnected();
				try
				{
					await Task.Delay(TimeSpan.FromSeconds(RetryIntervalSec), ct);
				}
				catch (OperationCanceledException)
				{
					return;
				}
			}
		}
	}

	private async Task EstablishAsync(CancellationToken ct)
	{
		_tcp = new TcpClient();
		await _tcp.ConnectAsync(Info.Host, Info.Port, ct);
		_stream = _tcp.GetStream();

		await _stream.WriteAsync(
			PacketSerializer.Serialize(PacketId.Ping, new PingRequest{}), ct);

		var resp = await PacketSerializer.ReadPacketAsync(_stream, ct);
		if (null == resp || resp.Value.id != PacketId.Pong)
			throw new InvalidOperationException("Auth failed");

		IsConnected = true;
		NotifyStateChanged();
	}

	private async Task HeartbeatLoopAsync(CancellationToken ct)
	{
		while (!ct.IsCancellationRequested)
		{
			await Task.Delay(TimeSpan.FromSeconds(HeartbeatIntervalSec), ct);

			await _lock.WaitAsync(ct);
			try
			{
				await _stream!.WriteAsync(PacketSerializer.SerializeEmpty(PacketId.GetResource), ct);
				var resp = await PacketSerializer.ReadPacketAsync(_stream, ct);

				if (null == resp)
					throw new IOException("Connection lost during heartbeat");

				if (resp.Value.id == PacketId.ResourceInfo)
					LastResource = PacketSerializer.Deserialize<ResourceInfoResponse>(resp.Value.payload);

				LastHeartbeatAt = DateTime.UtcNow;
				NotifyStateChanged();
			}
			finally
			{
				_lock.Release();
			}
		}
	}

	/// <summary>
	/// Sends a typed request to the Agent and receives a response.
	/// Returns null if not connected or if the request times out.
	/// </summary>
	/// <typeparam name="T">Type of request payload</typeparam>
	/// <param name="id">Packet ID for the request</param>
	/// <param name="payload">Request payload object</param>
	/// <param name="ct">Cancellation token</param>
	/// <returns>Response packet tuple or null if unavailable</returns>
	public async Task<(PacketId id, byte[] payload, byte key)?> SendAsync<T>(
		PacketId id, T payload, CancellationToken ct = default)
	{
		if (!IsConnected)
			return null;

		bool acquired = await _lock.WaitAsync(TimeSpan.FromSeconds(10), ct);
		if (!acquired)
			return null;

		try
		{
			await _stream!.WriteAsync(PacketSerializer.Serialize(id, payload), ct);
			return await PacketSerializer.ReadPacketAsync(_stream, ct);
		}
		catch
		{
			SetDisconnected();
			return null;
		}
		finally
		{
			_lock.Release();
		}
	}

	/// <summary>
	/// Sends a header-only request (no payload) and receives a response.
	/// </summary>
	/// <param name="id">Packet ID for the request</param>
	/// <param name="ct">Cancellation token</param>
	/// <returns>Response packet tuple or null if unavailable</returns>
	public async Task<(PacketId id, byte[] payload, byte key)?> SendEmptyAsync(
		PacketId id, CancellationToken ct = default)
	{
		if (!IsConnected)
			return null;

		bool acquired = await _lock.WaitAsync(TimeSpan.FromSeconds(10), ct);
		if (!acquired)
			return null;

		try
		{
			await _stream!.WriteAsync(PacketSerializer.SerializeEmpty(id), ct);
			return await PacketSerializer.ReadPacketAsync(_stream, ct);
		}
		catch
		{
			SetDisconnected();
			return null;
		}
		finally
		{
			_lock.Release();
		}
	}

	/// <summary>
	/// Retrieves external services status (MySQL, MSSQL, Redis, etc.) from the Agent.
	/// </summary>
	/// <param name="ct">Cancellation token</param>
	/// <returns>External services information or null if not available</returns>
	public async Task<ExternalServicesInfoResponse?> GetExternalServicesAsync(CancellationToken ct = default)
	{
		var resp = await SendEmptyAsync(PacketId.GetExternalServices, ct);
		if (null == resp)
			return null;

		return PacketSerializer.Deserialize<ExternalServicesInfoResponse>(resp.Value.payload);
	}

	/// <summary>
	/// 감시 중인 프로세스 상태를 에이전트에서 검색합니다.
	/// </summary>
	/// <param name="ct">Cancellation token</param>
	/// <returns>감시 프로세스 정보 또는 사용 불가능한 경우 null</returns>
	public async Task<MonitoredProcessesResponse?> GetMonitoredProcessesAsync(CancellationToken ct = default)
	{
		var resp = await SendEmptyAsync(PacketId.GetMonitoredProcesses, ct);
		if (null == resp)
			return null;

		return PacketSerializer.Deserialize<MonitoredProcessesResponse>(resp.Value.payload);
	}

	/// <summary>
	/// 감시 프로세스 설정을 추가하거나 수정합니다.
	/// </summary>
	/// <param name="config">추가/수정할 프로세스 설정</param>
	/// <param name="oldName">기존 프로세스 이름 (수정하는 경우)</param>
	/// <param name="ct">Cancellation token</param>
	/// <returns>성공/실패 응답</returns>
	public async Task<ResultResponse?> UpsertMonitoredProcessAsync(MonitoredProcessConfig config, string? oldName = null, CancellationToken ct = default)
	{
		var req = new UpsertMonitoredProcessRequest { Config = config, OldName = oldName };
		var resp = await SendAsync(PacketId.UpsertMonitoredProcess, req, ct);
		if (null == resp)
			return null;

		return PacketSerializer.Deserialize<ResultResponse>(resp.Value.payload);
	}

	/// <summary>
	/// 감시 프로세스 설정을 삭제합니다.
	/// </summary>
	/// <param name="name">삭제할 프로세스 이름</param>
	/// <param name="ct">Cancellation token</param>
	/// <returns>성공/실패 응답</returns>
	public async Task<ResultResponse?> DeleteMonitoredProcessAsync(string name, CancellationToken ct = default)
	{
		var req = new DeleteMonitoredProcessRequest { Name = name };
		var resp = await SendAsync(PacketId.DeleteMonitoredProcess, req, ct);
		if (null == resp)
			return null;

		return PacketSerializer.Deserialize<ResultResponse>(resp.Value.payload);
	}

	/// <summary>
	/// 감시 프로세스를 재시작합니다.
	/// </summary>
	/// <param name="name">재시작할 프로세스 이름</param>
	/// <param name="ct">Cancellation token</param>
	/// <returns>성공/실패 응답</returns>
	public async Task<ResultResponse?> RestartMonitoredProcessAsync(string name, CancellationToken ct = default)
	{
		var req = new RestartMonitoredProcessRequest { Name = name };
		var resp = await SendAsync(PacketId.RestartMonitoredProcess, req, ct);
		if (null == resp)
			return null;

		return PacketSerializer.Deserialize<ResultResponse>(resp.Value.payload);
	}

	/// <summary>
	/// 감시 프로세스를 시작합니다.
	/// </summary>
	/// <param name="uid">시작할 프로세스의 UUID</param>
	/// <param name="ct">Cancellation token</param>
	/// <returns>성공/실패 응답</returns>
	public async Task<ResultResponse?> StartMonitoredProcessAsync(string uid, CancellationToken ct = default)
	{
		var req = new StartMonitoredProcessRequest { Uid = uid };
		var resp = await SendAsync(PacketId.StartMonitoredProcess, req, ct);
		if (null == resp)
			return null;

		return PacketSerializer.Deserialize<ResultResponse>(resp.Value.payload);
	}

	/// <summary>
	/// 감시 프로세스를 중지합니다.
	/// </summary>
	/// <param name="uid">중지할 프로세스의 UUID</param>
	/// <param name="ct">Cancellation token</param>
	/// <returns>성공/실패 응답</returns>
	public async Task<ResultResponse?> StopMonitoredProcessAsync(string uid, CancellationToken ct = default)
	{
		var req = new StopMonitoredProcessRequest { Uid = uid };
		var resp = await SendAsync(PacketId.StopMonitoredProcess, req, ct);
		if (null == resp)
			return null;

		return PacketSerializer.Deserialize<ResultResponse>(resp.Value.payload);
	}

	private void SetDisconnected()
	{
		IsConnected = false;
		_stream?.Dispose();
		_tcp?.Dispose();
		_stream = null;
		_tcp = null;
		NotifyStateChanged();
	}

	private void NotifyStateChanged()
	{
		try { OnStateChanged?.Invoke(); } catch { }
	}

	/// <summary>
	/// Cleans up the connection and resources.
	/// </summary>
	/// <returns>Completion task</returns>
	public async ValueTask DisposeAsync()
	{
		_cts.Cancel();
		try { await _runLoop; } catch { }
		_cts.Dispose();
		_lock.Dispose();
		_stream?.Dispose();
		_tcp?.Dispose();
	}
}
