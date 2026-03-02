using DeployTool.Common.Packets;
using MySqlConnector;
using StackExchange.Redis;
using System.Data.SqlClient;
using System.Diagnostics;

namespace DeployTool.Agent.Services;

/// <summary>
/// MySQL, MSSQL, Redis의 외부 데이터베이스 및 캐시 서비스의 상태와 연결을 모니터링합니다.
/// </summary>
public class ExternalServiceMonitor
{
	private const int ConnectionTimeoutMs = 3000;
	private static readonly Dictionary<string, ExternalServiceStatus> _cache = new();
	private static readonly object _cacheLock = new();

	private readonly Dictionary<string, ServiceConnectionConfig> _services;

	/// <summary>
	/// ExternalServiceMonitor 클래스의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="services">appsettings의 외부 서비스 설정</param>
	public ExternalServiceMonitor(Dictionary<string, ServiceConnectionConfig> services)
	{
		_services = services ?? new();
	}

	/// <summary>
	/// 사용 가능한 경우 캐시를 사용하여 설정된 모든 외부 서비스의 현재 상태를 가져옵니다.
	/// </summary>
	/// <returns>서비스 상태 목록</returns>
	public async Task<List<ExternalServiceStatus>> GetAllServicesAsync()
	{
		var result = new List<ExternalServiceStatus>();

		foreach (var (name, config) in _services)
		{
			var status = name.ToLower() switch
			{
				"mysql" => await CheckMySqlAsync(name, config),
				"mssql" => await CheckMssqlAsync(name, config),
				"redis" => await CheckRedisAsync(name, config),
				_ => new ExternalServiceStatus
				{
					ServiceType = name,
					DisplayName = name,
					Host = config.Host,
					Port = config.Port,
					Status = "Unknown",
					ErrorMessage = $"Unknown service type: {name}",
					LastCheckUtc = DateTime.UtcNow
				}
			};

			result.Add(status);
		}

		return result;
	}

	/// <summary>
	/// MySQL 서버 연결성을 확인하고 상태를 반환합니다.
	/// </summary>
	private async Task<ExternalServiceStatus> CheckMySqlAsync(string name, ServiceConnectionConfig config)
	{
		var status = new ExternalServiceStatus
		{
			ServiceType = "MySQL",
			DisplayName = name,
			Host = config.Host,
			Port = config.Port,
			LastCheckUtc = DateTime.UtcNow
		};

		var sw = Stopwatch.StartNew();

		try
		{
			var connStr = $"Server={config.Host};Port={config.Port};User={config.Username};Password={config.Password};Database={config.Database ?? "mysql"};Connection Timeout={ConnectionTimeoutMs / 1000};";
			using var conn = new MySqlConnection(connStr);

			using var cts = new CancellationTokenSource(ConnectionTimeoutMs);
			await conn.OpenAsync(cts.Token);

			sw.Stop();
			status.Status = "Connected";
			status.ResponseTimeMs = (int)sw.ElapsedMilliseconds;
		}
		catch (OperationCanceledException)
		{
			sw.Stop();
			status.Status = "Timeout";
			status.ResponseTimeMs = ConnectionTimeoutMs;
			status.ErrorMessage = "Connection timeout";
		}
		catch (MySqlException ex) when (ex.Message.Contains("Access denied"))
		{
			sw.Stop();
			status.Status = "AuthFailed";
			status.ResponseTimeMs = (int)sw.ElapsedMilliseconds;
			status.ErrorMessage = "Authentication failed";
		}
		catch (Exception ex)
		{
			sw.Stop();
			status.Status = "Offline";
			status.ResponseTimeMs = (int)sw.ElapsedMilliseconds;
			status.ErrorMessage = ex.Message;
		}

		return status;
	}

	/// <summary>
	/// MSSQL 서버 연결성을 확인하고 상태를 반환합니다.
	/// </summary>
	private async Task<ExternalServiceStatus> CheckMssqlAsync(string name, ServiceConnectionConfig config)
	{
		var status = new ExternalServiceStatus
		{
			ServiceType = "MSSQL",
			DisplayName = name,
			Host = config.Host,
			Port = config.Port,
			LastCheckUtc = DateTime.UtcNow
		};

		var sw = Stopwatch.StartNew();

		try
		{
			var connStr = $"Server={config.Host},{config.Port};User Id={config.Username};Password={config.Password};Database={config.Database ?? "master"};Connection Timeout={ConnectionTimeoutMs / 1000};";
			using var conn = new SqlConnection(connStr);

			using var cts = new CancellationTokenSource(ConnectionTimeoutMs);
			await conn.OpenAsync(cts.Token);

			sw.Stop();
			status.Status = "Connected";
			status.ResponseTimeMs = (int)sw.ElapsedMilliseconds;
		}
		catch (OperationCanceledException)
		{
			sw.Stop();
			status.Status = "Timeout";
			status.ResponseTimeMs = ConnectionTimeoutMs;
			status.ErrorMessage = "Connection timeout";
		}
		catch (SqlException ex) when (ex.Number == 18456)
		{
			sw.Stop();
			status.Status = "AuthFailed";
			status.ResponseTimeMs = (int)sw.ElapsedMilliseconds;
			status.ErrorMessage = "Authentication failed";
		}
		catch (Exception ex)
		{
			sw.Stop();
			status.Status = "Offline";
			status.ResponseTimeMs = (int)sw.ElapsedMilliseconds;
			status.ErrorMessage = ex.Message;
		}

		return status;
	}

	/// <summary>
	/// Redis 서버 연결성을 확인하고 상태를 반환합니다.
	/// </summary>
	private async Task<ExternalServiceStatus> CheckRedisAsync(string name, ServiceConnectionConfig config)
	{
		var status = new ExternalServiceStatus
		{
			ServiceType = "Redis",
			DisplayName = name,
			Host = config.Host,
			Port = config.Port,
			LastCheckUtc = DateTime.UtcNow
		};

		var sw = Stopwatch.StartNew();

		try
		{
			var options = ConfigurationOptions.Parse($"{config.Host}:{config.Port}");
			options.ConnectTimeout = ConnectionTimeoutMs;
			options.SyncTimeout = ConnectionTimeoutMs;

			using var redis = await ConnectionMultiplexer.ConnectAsync(options);

			if (redis.IsConnected)
			{
				// 실제 연결성 확인을 위한 Ping
				var db = redis.GetDatabase();
				using var cts = new CancellationTokenSource(ConnectionTimeoutMs);
				await db.PingAsync();

				sw.Stop();
				status.Status = "Connected";
				status.ResponseTimeMs = (int)sw.ElapsedMilliseconds;
			}
			else
			{
				sw.Stop();
				status.Status = "Offline";
				status.ResponseTimeMs = (int)sw.ElapsedMilliseconds;
				status.ErrorMessage = "Connection failed";
			}
		}
		catch (OperationCanceledException)
		{
			sw.Stop();
			status.Status = "Timeout";
			status.ResponseTimeMs = ConnectionTimeoutMs;
			status.ErrorMessage = "Connection timeout";
		}
		catch (Exception ex)
		{
			sw.Stop();
			status.Status = "Offline";
			status.ResponseTimeMs = (int)sw.ElapsedMilliseconds;
			status.ErrorMessage = ex.Message;
		}

		return status;
	}
}
