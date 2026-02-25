namespace DeployTool.Protocol.Packets;

// ── Manager → Agent ───────────────────────────────────────────
public class SetWorkDirRequest  { public string Path { get; set; } = ""; }
public class ListFilesRequest   { public string Path { get; set; } = ""; } // 비어 있으면 WorkDir
public class GetFileRequest     { public string Path { get; set; } = ""; }
public class DeleteFileRequest  { public string Path { get; set; } = ""; }
public class MakeDirRequest     { public string Path { get; set; } = ""; }
public class GetFileHashRequest { public string Path { get; set; } = ""; }
public class BackupDirRequest   { public string SourcePath { get; set; } = ""; public string OutputPath { get; set; } = ""; }

public class PutFileRequest
{
	public string Path      { get; set; } = "";
	public string DataBase64 { get; set; } = ""; // Base64 인코딩된 파일 내용
	public bool   Overwrite { get; set; } = true;
}

public class MoveFileRequest
{
	public string SourcePath { get; set; } = "";
	public string DestPath   { get; set; } = "";
}

public class SyncDirRequest
{
	public string                  Path  { get; set; } = "";
	public List<FileSyncEntry> LocalFiles { get; set; } = new();
}

public class FileSyncEntry
{
	public string RelativePath { get; set; } = "";
	public string Hash         { get; set; } = "";
}

public class CompressRequest
{
	public string SourcePath { get; set; } = "";
	public string OutputPath { get; set; } = "";
	public string Format     { get; set; } = "zip"; // "zip" | "targz"
}

public class DecompressRequest
{
	public string SourcePath { get; set; } = "";
	public string OutputPath { get; set; } = "";
}

// ── Agent → Manager ───────────────────────────────────────────
public class FileEntry
{
	public string Name         { get; set; } = "";
	public long   Size         { get; set; }
	public bool   IsDir        { get; set; }
	public long   ModifiedUnix { get; set; }
}

public class FileListResponse
{
	public string          Path  { get; set; } = "";
	public List<FileEntry> Files { get; set; } = new();
}

public class FileDataResponse
{
	public string Path      { get; set; } = "";
	public string DataBase64 { get; set; } = "";
}

public class FileHashResponse
{
	public string Path { get; set; } = "";
	public string Hash { get; set; } = ""; // SHA-256 hex
}

public class SyncDiffResponse
{
	public string       Path    { get; set; } = "";
	public List<string> Missing { get; set; } = new(); // 원격에 없는 파일
	public List<string> Changed { get; set; } = new(); // 해시 불일치 파일
	public List<string> Extra   { get; set; } = new(); // 원격에만 있는 파일
}
