namespace DeployTool.Common.Packets;

// ── 매니저 → 에이전트 ───────────────────────────────────────────
/// <summary>작업 디렉터리를 변경하기 위한 요청</summary>
public class SetWorkDirRequest  { 
	/// <summary>대상 디렉터리 경로</summary>
	public string Path { get; set; } = ""; 
}

/// <summary>디렉터리의 파일을 나열하기 위한 요청</summary>
public class ListFilesRequest   { 
	/// <summary>나열할 디렉터리 경로 (비어 있음 = 작업 디렉터리)</summary>
	public string Path { get; set; } = ""; 
}

/// <summary>파일을 다운로드하기 위한 요청</summary>
public class GetFileRequest     { 
	/// <summary>다운로드할 파일 경로</summary>
	public string Path { get; set; } = ""; 
}

/// <summary>파일 또는 디렉터리를 삭제하기 위한 요청</summary>
public class DeleteFileRequest  { 
	/// <summary>삭제할 경로</summary>
	public string Path { get; set; } = ""; 
}

/// <summary>디렉터리를 생성하기 위한 요청</summary>
public class MakeDirRequest     { 
	/// <summary>생성할 디렉터리 경로</summary>
	public string Path { get; set; } = ""; 
}

/// <summary>파일 해시를 계산하기 위한 요청</summary>
public class GetFileHashRequest { 
	/// <summary>해시할 파일 경로</summary>
	public string Path { get; set; } = ""; 
}

/// <summary>디렉터리를 ZIP 아카이브로 백업하기 위한 요청</summary>
public class BackupDirRequest   { 
	/// <summary>백업할 원본 디렉터리</summary>
	public string SourcePath { get; set; } = ""; 
	/// <summary>출력 ZIP 파일 경로</summary>
	public string OutputPath { get; set; } = ""; 
}

/// <summary>Base64 인코딩 내용으로 파일을 업로드하기 위한 요청</summary>
public class PutFileRequest
{
	/// <summary>대상 파일 경로/이름</summary>
	public string Path      { get; set; } = "";
	/// <summary>Base64 인코딩 파일 내용</summary>
	public string DataBase64 { get; set; } = "";
	/// <summary>true이면 기존 파일을 덮어쓰기</summary>
	public bool   Overwrite { get; set; } = true;
}

/// <summary>파일/디렉터리를 이동하거나 이름을 바꾸기 위한 요청</summary>
public class MoveFileRequest
{
	/// <summary>원본 파일/디렉터리 경로</summary>
	public string SourcePath { get; set; } = "";
	/// <summary>대상 경로</summary>
	public string DestPath   { get; set; } = "";
}

/// <summary>동기화를 위해 로컬 및 원격 파일 해시를 비교하기 위한 요청</summary>
public class SyncDirRequest
{
	/// <summary>동기화할 디렉터리 경로</summary>
	public string                  Path  { get; set; } = "";
	/// <summary>해시가 있는 로컬 파일 목록</summary>
	public List<FileSyncEntry> LocalFiles { get; set; } = new();
}

/// <summary>동기화를 위한 상대 경로 및 SHA-256 해시가 있는 파일 항목</summary>
public class FileSyncEntry
{
	/// <summary>기본 디렉터리의 상대 파일 경로</summary>
	public string RelativePath { get; set; } = "";
	/// <summary>SHA-256 해시 (16진수 문자열)</summary>
	public string Hash         { get; set; } = "";
}

/// <summary>디렉터리를 압축하기 위한 요청</summary>
public class CompressRequest
{
	/// <summary>압축할 원본 디렉터리</summary>
	public string SourcePath { get; set; } = "";
	/// <summary>출력 아카이브 경로</summary>
	public string OutputPath { get; set; } = "";
	/// <summary>압축 형식: "zip" 또는 "targz"</summary>
	public string Format     { get; set; } = "zip";
}

/// <summary>아카이브를 압축 해제하기 위한 요청</summary>
public class DecompressRequest
{
	/// <summary>원본 아카이브 파일 경로</summary>
	public string SourcePath { get; set; } = "";
	/// <summary>추출된 파일의 출력 디렉터리</summary>
	public string OutputPath { get; set; } = "";
}

// ── 에이전트 → 매니저 ───────────────────────────────────────────
/// <summary>파일 목록에 대한 디렉터리 항목 메타데이터</summary>
public class FileEntry
{
	/// <summary>파일 또는 디렉터리 이름</summary>
	public string Name         { get; set; } = "";
	/// <summary>파일 크기(바이트) (디렉터리의 경우 0)</summary>
	public long   Size         { get; set; }
	/// <summary>이것이 디렉터리이면 true, 파일이면 false</summary>
	public bool   IsDir        { get; set; }
	/// <summary>Unix 타임스탬프로서의 마지막 수정 시간</summary>
	public long   ModifiedUnix { get; set; }
}

/// <summary>파일 메타데이터가 있는 디렉터리 목록을 포함하는 응답</summary>
public class FileListResponse
{
	/// <summary>나열된 디렉터리 경로</summary>
	public string          Path  { get; set; } = "";
	/// <summary>파일 및 하위 디렉터리 목록</summary>
	public List<FileEntry> Files { get; set; } = new();
}

/// <summary>Base64로 인코딩된 파일 내용을 포함하는 응답</summary>
public class FileDataResponse
{
	/// <summary>요청된 파일 경로</summary>
	public string Path      { get; set; } = "";
	/// <summary>Base64 인코딩 파일 내용</summary>
	public string DataBase64 { get; set; } = "";
}

/// <summary>SHA-256 파일 해시를 포함하는 응답</summary>
public class FileHashResponse
{
	/// <summary>해시된 파일 경로</summary>
	public string Path { get; set; } = "";
	/// <summary>16진수 형식의 SHA-256 해시</summary>
	public string Hash { get; set; } = "";
}

/// <summary>누락되고 변경되고 추가된 파일 목록이 있는 동기화 결과</summary>
public class SyncDiffResponse
{
	/// <summary>비교된 디렉터리 경로</summary>
	public string       Path    { get; set; } = "";
	/// <summary>로컬 매니페스트에는 있지만 원격에는 없는 파일</summary>
	public List<string> Missing { get; set; } = new();
	/// <summary>해시가 다른 파일 (업데이트 필요)</summary>
	public List<string> Changed { get; set; } = new();
	/// <summary>원격에는 있지만 로컬 매니페스트에는 없는 파일 (추가)</summary>
	public List<string> Extra   { get; set; } = new();
}
