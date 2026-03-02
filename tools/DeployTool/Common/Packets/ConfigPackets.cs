namespace DeployTool.Common.Packets;

/// <summary>로그 파일의 마지막 N개 라인을 가져오기 위한 요청</summary>
public class GetLogRequest
{
	/// <summary>로그 파일의 경로</summary>
	public string Path  { get; set; } = "";
	/// <summary>반환할 라인의 수 (기본값: 100)</summary>
	public int    Lines { get; set; } = 100;
}

/// <summary>실시간 로그 스트리밍을 시작하기 위한 요청</summary>
public class WatchLogRequest { 
	/// <summary>스트리밍할 로그 파일의 경로</summary>
	public string Path { get; set; } = ""; 
}

/// <summary>로그 스트리밍을 중지하기 위한 요청</summary>
public class StopWatchLogRequest { }

/// <summary>셸 명령을 실행하기 위한 요청</summary>
public class ShellRequest
{
	/// <summary>실행할 셸 명령</summary>
	public string       Command { get; set; } = "";
	/// <summary>명령 인수</summary>
	public List<string> Args    { get; set; } = new();
	/// <summary>명령 실행을 위한 작업 디렉터리</summary>
	public string       WorkDir { get; set; } = "";
}

/// <summary>선택적 스트리밍 메타데이터가 있는 로그 파일 tail 응답</summary>
public class LogDataResponse
{
	/// <summary>로그 파일의 경로</summary>
	public string Path    { get; set; } = "";
	/// <summary>로그 내용 (tail 결과 또는 스트리밍 청크)</summary>
	public string Content { get; set; } = "";
	/// <summary>이것이 실시간 스트리밍 청크이면 true, tail 결과이면 false</summary>
	public bool   IsStream { get; set; } = false;
}

/// <summary>출력 및 종료 코드가 있는 셸 명령 실행 결과</summary>
public class ShellOutputResponse
{
	/// <summary>표준 출력 스트림 내용</summary>
	public string Stdout   { get; set; } = "";
	/// <summary>표준 오류 스트림 내용</summary>
	public string Stderr   { get; set; } = "";
	/// <summary>프로세스 종료 코드</summary>
	public int    ExitCode { get; set; }
}

/// <summary>JSON 경로 표기법을 사용한 설정 업데이트 요청</summary>
public class SetConfigRequest
{
	/// <summary>키는 콜론으로 구분된 JSON 경로를 사용하는 키:값 쌍의 사전 (예: "DeployAgent:WorkDir")</summary>
	public Dictionary<string, string> Changes { get; set; } = new();
}

/// <summary>전체 JSON 내용이 있는 설정 파일 응답</summary>
public class ConfigDataResponse
{
	/// <summary>완전한 appsettings.json 파일 내용</summary>
	public string Json { get; set; } = "";
}
