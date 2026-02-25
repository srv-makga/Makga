using System.Net.Sockets;
using DeployTool.Protocol;
using DeployTool.Protocol.Models;
using DeployTool.Protocol.Net;
using DeployTool.Protocol.Packets;

namespace DeployTool.Manager.Services;

// @brief Agent 1개와의 TCP 연결 관리 + 명령 송수신
public class AgentClient : IDisposable
{
	public ServerEntry Info       { get; }
	public bool        IsConnected => _tcp?.Connected ?? false;

	private TcpClient?    _tcp;
	private NetworkStream? _stream;
	private readonly SemaphoreSlim _lock = new(1, 1);

	public AgentClient(ServerEntry info) => Info = info;

	// @brief 연결 + Ping 인증
	public async Task<bool> ConnectAsync(CancellationToken ct = default)
	{
		await _lock.WaitAsync(ct);
		try
		{
			if (IsConnected)
				return true;

			_tcp    = new TcpClient();
			await _tcp.ConnectAsync(Info.Host, Info.Port, ct);
			_stream = _tcp.GetStream();

			// 인증
			var pingBytes = PacketSerializer.Serialize(PacketId.Ping, new PingRequest { Token = Info.Token });
			await _stream.WriteAsync(pingBytes, ct);

			var resp = await PacketSerializer.ReadPacketAsync(_stream, ct);
			if (null == resp || resp.Value.id != PacketId.Pong)
			{
				Disconnect();
				return false;
			}
			return true;
		}
		catch
		{
			Disconnect();
			return false;
		}
		finally
		{
			_lock.Release();
		}
	}

	// @brief 명령 전송 후 응답 반환 (연결 자동 시도)
	public async Task<(PacketId id, byte[] payload, byte key)?> SendAsync<T>(
		PacketId id, T payload, CancellationToken ct = default)
	{
		if (!IsConnected && !await ConnectAsync(ct))
			return null;

		await _lock.WaitAsync(ct);
		try
		{
			var bytes = PacketSerializer.Serialize(id, payload);
			await _stream!.WriteAsync(bytes, ct);
			return await PacketSerializer.ReadPacketAsync(_stream, ct);
		}
		catch
		{
			Disconnect();
			return null;
		}
		finally
		{
			_lock.Release();
		}
	}

	// @brief 페이로드 없는 명령 (GetInfo, ListProcess 등)
	public async Task<(PacketId id, byte[] payload, byte key)?> SendEmptyAsync(
		PacketId id, CancellationToken ct = default)
	{
		if (!IsConnected && !await ConnectAsync(ct))
			return null;

		await _lock.WaitAsync(ct);
		try
		{
			await _stream!.WriteAsync(PacketSerializer.SerializeEmpty(id), ct);
			return await PacketSerializer.ReadPacketAsync(_stream, ct);
		}
		catch
		{
			Disconnect();
			return null;
		}
		finally
		{
			_lock.Release();
		}
	}

	private void Disconnect()
	{
		_stream?.Dispose();
		_tcp?.Dispose();
		_stream = null;
		_tcp    = null;
	}

	public void Dispose()
	{
		Disconnect();
		_lock.Dispose();
	}
}
