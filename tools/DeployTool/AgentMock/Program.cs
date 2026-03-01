using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text.Json;
using DeployTool.Common;
using DeployTool.Common.Net;
using DeployTool.Common.Packets;

// ─── 설정 로드 (appsettings.json → 커맨드라인 오버라이드) ───────
string ip    = "0.0.0.0";
int    port  = 7700;
string token = "secret1";
string name  = "mock-agent";

var cfgPath = Path.Combine(AppContext.BaseDirectory, "appsettings.json");
if (File.Exists(cfgPath))
{
	using var doc = JsonDocument.Parse(File.ReadAllText(cfgPath));
	var root = doc.RootElement;
	JsonElement prop;
	if (root.TryGetProperty("ip",    out prop)) ip    = prop.GetString() ?? ip;
	if (root.TryGetProperty("port",  out prop)) port  = prop.GetInt32();
	if (root.TryGetProperty("token", out prop)) token = prop.GetString() ?? token;
	if (root.TryGetProperty("name",  out prop)) name  = prop.GetString() ?? name;
}

for (var i = 0; i < args.Length - 1; i++)
{
	if (args[i] == "--ip")    ip    = args[i + 1];
	if (args[i] == "--port")  port  = int.Parse(args[i + 1]);
	if (args[i] == "--token") token = args[i + 1];
	if (args[i] == "--name")  name  = args[i + 1];
}

// ─── TCP 리스너 시작 ──────────────────────────────────────────
Console.WriteLine($"[AgentMock] '{name}' listening on {ip}:{port}  (token={token})");
Console.WriteLine("[AgentMock] Press Ctrl+C to stop.");

var startTime = DateTime.UtcNow;
var listener  = new TcpListener(IPAddress.Parse(ip), port);
var sessions  = new Dictionary<string, (MockSession Session, Task RunTask)>();
var sessLock  = new object();
listener.Start();

var cts = new CancellationTokenSource();
Console.CancelKeyPress += (_, e) => { e.Cancel = true; cts.Cancel(); };

while (!cts.Token.IsCancellationRequested)
{
	TcpClient client;
	try
	{
		client = await listener.AcceptTcpClientAsync(cts.Token);
	}
	catch (OperationCanceledException)
	{
		break;
	}
	catch (Exception ex)
	{
		Console.WriteLine($"[AgentMock] Accept error: {ex.Message}");
		continue;
	}

	var remoteIp = ((System.Net.IPEndPoint)client.Client.RemoteEndPoint!).Address.ToString();

	lock (sessLock)
	{
		// 살아있는 세션이 있으면 중복 접속 거절
		if (sessions.TryGetValue(remoteIp, out var existing) && !existing.RunTask.IsCompleted)
		{
			Console.WriteLine($"[AgentMock] Rejecting duplicate connection from {remoteIp}");
			client.Dispose();
			continue;
		}

		// 이전 세션이 죽었거나 없으면 교체
		existing.Session?.Dispose();

		var session = new MockSession(client, token, name, startTime);
		var runTask = session.RunAsync(cts.Token);
		sessions[remoteIp] = (session, runTask);
		Console.WriteLine($"[AgentMock] New session from {remoteIp}");

		// 세션 종료 후 dict 에서 제거 (동일 세션일 때만 — race 방지)
		_ = runTask.ContinueWith(_ =>
		{
			lock (sessLock)
			{
				if (sessions.TryGetValue(remoteIp, out var cur) &&
				    ReferenceEquals(cur.Session, session))
					sessions.Remove(remoteIp);
			}
		}, TaskContinuationOptions.ExecuteSynchronously);
	}
}

lock (sessLock)
{
	foreach (var (s, _) in sessions.Values)
		s.Dispose();
	sessions.Clear();
}
listener.Stop();
Console.WriteLine("[AgentMock] Stopped.");

// ─── MockSession: 단일 Manager 연결을 전담 관리 ──────────────
sealed class MockSession : IDisposable
{
	private readonly TcpClient  _tcp;
	private readonly string     _token;
	private readonly string     _name;
	private readonly DateTime   _startTime;
	private readonly CancellationTokenSource _cts = new();

	public MockSession(TcpClient tcp, string token, string name, DateTime startTime)
	{
		_tcp       = tcp;
		_token     = token;
		_name      = name;
		_startTime = startTime;
	}

	public async Task RunAsync(CancellationToken externalCt)
	{
		using var linked = CancellationTokenSource.CreateLinkedTokenSource(externalCt, _cts.Token);
		var ct     = linked.Token;
		var remote = _tcp.Client.RemoteEndPoint?.ToString() ?? "unknown";
		Console.WriteLine($"[AgentMock] Connected: {remote}");

		{
			var stream = _tcp.GetStream();
			var authed = false;
			try
			{
				while (!ct.IsCancellationRequested)
				{
					var packet = await PacketSerializer.ReadPacketAsync(stream, ct);
					if (null == packet)
						break;

					var (id, payload, _) = packet.Value;

					if (!authed)
					{
						if (id != PacketId.Ping)
						{
							Console.WriteLine($"[AgentMock] First packet was {id}, not Ping. Closing.");
							break;
						}

						var ping = PacketSerializer.Deserialize<PingRequest>(payload);
						if (null == ping || ping.Token != _token)
						{
							await stream.WriteAsync(PacketSerializer.Serialize(
								PacketId.Result,
								new ResultResponse { Success = false, Message = "Unauthorized", Code = 401 }), ct);
							break;
						}

						authed = true;
						var uptime = (ulong)(DateTime.UtcNow - _startTime).TotalSeconds;
						await stream.WriteAsync(PacketSerializer.Serialize(
							PacketId.Pong, new PongResponse { UptimeSec = uptime }), ct);
						Console.WriteLine($"[AgentMock] Authenticated: {remote}");
						continue;
					}

					Console.WriteLine($"[AgentMock] Command: {id}");
					var response = HandleCommand(id, payload);
					if (null != response)
						await stream.WriteAsync(response, ct);
				}
			}
			catch (Exception ex) when (ex is not OperationCanceledException)
			{
				Console.WriteLine($"[AgentMock] Error [{remote}]: {ex.Message}");
			}
		}
	}

	private byte[]? HandleCommand(PacketId id, byte[] payload)
	{
		return id switch
		{
			PacketId.GetInfo => PacketSerializer.Serialize(PacketId.AgentInfo, new AgentInfoResponse
			{
				Version   = "1.0.0-mock",
				Os        = RuntimeInformation.OSDescription,
				Hostname  = $"{_name}-host",
				WorkDir   = "/mock/workdir",
				UptimeSec = (ulong)(DateTime.UtcNow - _startTime).TotalSeconds
			}),

			PacketId.GetResource => PacketSerializer.Serialize(PacketId.ResourceInfo, new ResourceInfoResponse
			{
				CpuPct      = (float)(Random.Shared.NextDouble() * 80),
				MemUsedMb   = 2048 + Random.Shared.Next(-256, 256),
				MemTotalMb  = 8192,
				DiskUsedGb  = 50.0 + Random.Shared.NextDouble() * 5,
				DiskTotalGb = 200.0,
				Os          = RuntimeInformation.OSDescription
			}),

			PacketId.GetNetwork => PacketSerializer.Serialize(PacketId.NetworkInfo, new NetworkInfoResponse
			{
				Interfaces = new List<NetworkInterfaceEntry>
				{
					new() { Name = "eth0", BytesSent = 1_000_000 + Random.Shared.Next(0, 100_000), BytesRecv = 5_000_000 + Random.Shared.Next(0, 500_000) },
					new() { Name = "lo",   BytesSent = 50_000,                                      BytesRecv = 50_000 }
				}
			}),

			PacketId.ListFiles => PacketSerializer.Serialize(PacketId.FileList, new FileListResponse
			{
				Path  = "/mock/workdir",
				Files = new List<FileEntry>
				{
					new() { Name = "mock_server.exe", IsDir = false, Size = 1_024_000, ModifiedUnix = DateTimeOffset.UtcNow.AddDays(-1).ToUnixTimeSeconds() },
					new() { Name = "config",          IsDir = true,  Size = 0,         ModifiedUnix = DateTimeOffset.UtcNow.AddDays(-7).ToUnixTimeSeconds() },
					new() { Name = "logs",            IsDir = true,  Size = 0,         ModifiedUnix = DateTimeOffset.UtcNow.AddHours(-1).ToUnixTimeSeconds() }
				}
			}),

			PacketId.Shell => PacketSerializer.Serialize(PacketId.ShellOutput, new ResultResponse
			{
				Success = true,
				Message = "[mock] shell command executed"
			}),

			PacketId.PutFile => PacketSerializer.Serialize(PacketId.Result, new ResultResponse
			{
				Success = true,
				Message = "[mock] file received"
			}),

			_ => PacketSerializer.Serialize(PacketId.Result, new ResultResponse
			{
				Success = true,
				Message = $"[mock] {id} acknowledged"
			})
		};
	}

	public void Dispose()
	{
		_cts.Cancel();
		_cts.Dispose();
		try { _tcp.Dispose(); } catch { }
	}
}

