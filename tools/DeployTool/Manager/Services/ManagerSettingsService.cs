using System.Text.Json;
using System.Text.Json.Nodes;
using Microsoft.Extensions.Options;

namespace DeployTool.Manager.Services;

/// <summary>
/// appsettings.json에 매니저 설정 지속성을 관리합니다.
/// 재시도 및 하트비트 간격 설정을 저장할 수 있습니다.
/// </summary>
public class ManagerSettingsService
{
	// .NET API: JsonSerializerOptions를 static readonly로 캐시하여 반복 생성 오버헤드 제거
	private static readonly JsonSerializerOptions PrettyJsonOptions = 
		new() { WriteIndented = true };

	private readonly string                         _path;
	private readonly IOptionsMonitor<ManagerConfig> _options;
	// 동시 파일 쓰기 경쟁 조건 방지: SemaphoreSlim으로 직렬화
	private readonly SemaphoreSlim _writeLock = new(1, 1);

	/// <summary>
	/// ManagerSettingsService 클래스의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="env">기본 경로 확인을 위한 웹 호스트 환경</param>
	/// <param name="options">설정 옵션 모니터</param>
	public ManagerSettingsService(IWebHostEnvironment env, IOptionsMonitor<ManagerConfig> options)
	{
		_path    = Path.Combine(env.ContentRootPath, "appsettings.json");
		_options = options;
	}

	/// <summary>
	/// 현재 매니저 설정 설정을 가져옵니다.
	/// </summary>
	public ManagerConfig Current => _options.CurrentValue;

	/// <summary>
	/// 재시도 간격 및 하트비트 간격 설정을 appsettings.json으로 저장합니다.
	/// 동시 파일 쓰기를 방지하기 위해 세마포어 잠금을 사용합니다.
	/// </summary>
	/// <param name="retryIntervalSec">재시도 간격(초)</param>
	/// <param name="heartbeatIntervalSec">하트비트 간격(초)</param>
	/// <returns>완료 작업</returns>
	public async Task SaveAsync(int retryIntervalSec, int heartbeatIntervalSec)
	{
		await _writeLock.WaitAsync();
		try
		{
			var text = await File.ReadAllTextAsync(_path);
			var node = JsonNode.Parse(text)
				?? throw new InvalidOperationException("appsettings.json parse failed");

			node["DeployManager"]!["RetryIntervalSec"]     = retryIntervalSec;
			node["DeployManager"]!["HeartbeatIntervalSec"] = heartbeatIntervalSec;

			await File.WriteAllTextAsync(_path, node.ToJsonString(PrettyJsonOptions));
		}
		finally
		{
			_writeLock.Release();
		}
	}
}
