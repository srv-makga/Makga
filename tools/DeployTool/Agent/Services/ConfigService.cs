using System.Text.Json;
using System.Text.Json.Nodes;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Configuration;

namespace DeployTool.Agent.Services;

/// <summary>
/// appsettings.json에 저장된 에이전트 설정을 관리하며 읽기, 쓰기 및 다시 로드 작업을 지원합니다.
/// </summary>
public class ConfigService
{
	// .NET API: JsonSerializerOptions를 static readonly로 캐시하여 반복 생성 오버헤드 제거
	private static readonly JsonSerializerOptions PrettyJsonOptions = 
		new() { WriteIndented = true };

	private readonly string _configPath;
	private readonly IConfiguration _configuration;
	// 동시 파일 쓰기 경쟁 조건 방지: SemaphoreSlim으로 직렬화
	private readonly SemaphoreSlim _writeLock = new(1, 1);

	/// <summary>
	/// ConfigService 클래스의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="configuration">설정 공급자</param>
	public ConfigService(IConfiguration configuration)
	{
		_configuration = configuration;
		_configPath    = Path.Combine(AppContext.BaseDirectory, "appsettings.json");
	}

	/// <summary>
	/// 전체 appsettings.json 설정을 JSON 문자열로 검색합니다.
	/// </summary>
	/// <returns>설정 JSON을 포함하는 응답</returns>
	public async Task<ConfigDataResponse> GetConfigAsync()
	{
		var json = await File.ReadAllTextAsync(_configPath);
		return new ConfigDataResponse { Json = json };
	}

	/// <summary>
	/// JSON 경로 표기법(콜론으로 구분된 키)을 사용하여 appsettings.json의 설정 값을 업데이트합니다.
	/// 여러 변경 사항은 동시 액세스 문제를 방지하는 파일 쓰기 동기화를 사용하여 원자적으로 적용됩니다.
	/// </summary>
	/// <param name="req">도트 표기법 JSON 경로가 있는 키:값 쌍을 포함하는 요청</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public async Task<ResultResponse> SetConfigAsync(SetConfigRequest req)
	{
		await _writeLock.WaitAsync();
		try
		{
			var json = await File.ReadAllTextAsync(_configPath);
			var root = JsonNode.Parse(json) as JsonObject
				?? throw new InvalidOperationException("Invalid appsettings.json");

			foreach (var (key, value) in req.Changes)
			{
				var parts   = key.Split(':');
				var current = root as JsonObject;

				for (var i = 0; i < parts.Length - 1; i++)
				{
					if (current![parts[i]] is not JsonObject child)
					{
						child = new JsonObject();
						current[parts[i]] = child;
					}
					current = child;
				}
				current![parts[^1]] = JsonValue.Create(value);
			}

			var updated = root.ToJsonString(PrettyJsonOptions);
			await File.WriteAllTextAsync(_configPath, updated);
			return new ResultResponse { Success = true };
		}
		finally
		{
			_writeLock.Release();
		}
	}

	/// <summary>
	/// appsettings.json에 최근 변경 사항을 반영하는 메모리 내 설정을 디스크에서 다시 로드합니다.
	/// </summary>
	/// <returns>성공을 나타내는 결과 응답</returns>
	public Task<ResultResponse> ReloadConfigAsync()
	{
		if (_configuration is IConfigurationRoot root)
			root.Reload();
		return Task.FromResult(new ResultResponse { Success = true, Message = "Config reloaded" });
	}
}
