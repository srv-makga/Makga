using System.Net.Sockets;
using DeployTool.Common;
using DeployTool.Common.Models;
using DeployTool.Common.Net;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Options;

namespace DeployTool.Manager.Services;

// @brief Agent 1개와의 지속 TCP 연결 관리.
//   - 생성 즉시 백그라운드 루프 시작 (연결 → 하트비트 → 재연결 반복)
//   - 하트비트마다 GetResource를 요청하고 LastResource에 캐싱
//   - RetryIntervalSec / HeartbeatIntervalSec 는 IOptionsMonitor 로 실시간 반영
public class AgentClient : IAsyncDisposable
{
	public  ServerEntry             Info            { get; }
	public  bool                    IsConnected     { get; private set; }
	public  ResourceInfoResponse?   LastResource    { get; private set; }
	public  DateTime?               LastHeartbeatAt { get; private set; }

	// UI 컴포넌트가 구독해 StateHasChanged 를 호출할 수 있도록 노출
	public event Action? OnStateChanged;

	private readonly IOptionsMonitor<ManagerConfig> _options;
	private int RetryIntervalSec     => _options.CurrentValue.RetryIntervalSec;
	private int HeartbeatIntervalSec => _options.CurrentValue.HeartbeatIntervalSec;

	private TcpClient?     _tcp;
	private NetworkStream? _stream;

	// 하트비트와 명령 송수신이 같은 소켓 스트림을 공유하므로 직렬화
	private readonly SemaphoreSlim _lock = new(1, 1);
	private readonly CancellationTokenSource _cts = new();

	public AgentClient(ServerEntry info, IOptionsMonitor<ManagerConfig> options)
	{
		Info     = info;
		_options = options;
		_ = RunLoopAsync(_cts.Token);
	}

	// ─── 백그라운드 연결 루프 ────────────────────────────────
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

	// @brief TCP 연결 + Ping 인증
	private async Task EstablishAsync(CancellationToken ct)
	{
		_tcp    = new TcpClient();
		await _tcp.ConnectAsync(Info.Host, Info.Port, ct);
		_stream = _tcp.GetStream();

		await _stream.WriteAsync(
			PacketSerializer.Serialize(PacketId.Ping, new PingRequest { Token = Info.Token }), ct);

		var resp = await PacketSerializer.ReadPacketAsync(_stream, ct);
		if (null == resp || resp.Value.id != PacketId.Pong)
			throw new InvalidOperationException("Auth failed");

		IsConnected = true;
		NotifyStateChanged();
	}

	// @brief 연결된 상태에서 HeartbeatIntervalSec 마다 GetResource 를 호출해 리소스 캐싱
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

	// ─── 명령 송수신 (UI → Agent) ────────────────────────────
	// @brief 페이로드 있는 명령 전송
	public async Task<(PacketId id, byte[] payload, byte key)?> SendAsync<T>(
		PacketId id, T payload, CancellationToken ct = default)
	{
		if (!IsConnected)
			return null;

		await _lock.WaitAsync(ct);
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

	// @brief 페이로드 없는 명령 전송
	public async Task<(PacketId id, byte[] payload, byte key)?> SendEmptyAsync(
		PacketId id, CancellationToken ct = default)
	{
		if (!IsConnected)
			return null;

		await _lock.WaitAsync(ct);
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

	// ─── 내부 유틸 ───────────────────────────────────────────
	private void SetDisconnected()
	{
		IsConnected = false;
		_stream?.Dispose();
		_tcp?.Dispose();
		_stream = null;
		_tcp    = null;
		NotifyStateChanged();
	}

	private void NotifyStateChanged()
	{
		try { OnStateChanged?.Invoke(); } catch { }
	}

	public ValueTask DisposeAsync()
	{
		_cts.Cancel();
		_cts.Dispose();
		_lock.Dispose();
		_stream?.Dispose();
		_tcp?.Dispose();
		return ValueTask.CompletedTask;
	}
}
