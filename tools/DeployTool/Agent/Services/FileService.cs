using System.IO.Compression;
using System.Security.Cryptography;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Options;

namespace DeployTool.Agent.Services;

/// <summary>
/// 목록, 읽기, 쓰기, 삭제, 이동, 압축 및 동기화를 포함한 파일 작업을 제공합니다.
/// </summary>
public class FileService
{
	private readonly AgentConfig _cfg;
	private string _workDir;

	/// <summary>
	/// FileService 클래스의 새 인스턴스를 초기화합니다.
	/// </summary>
	/// <param name="cfg">에이전트 설정 옵션</param>
	public FileService(IOptions<AgentConfig> cfg)
	{
		_cfg     = cfg.Value;
		_workDir = _cfg.WorkDir;
	}

	// @brief 패치 파일의 저장 루트. PatchRootDir 이 비어있으면 WorkDir 를 사용
	private string PatchRoot =>
		string.IsNullOrWhiteSpace(_cfg.PatchRootDir) ? _workDir : _cfg.PatchRootDir;

	/// <summary>
	/// 작업 디렉터리를 지정된 경로로 변경하여 해당 경로가 존재하는지 확인합니다.
	/// </summary>
	/// <param name="req">대상 디렉터리 경로를 포함하는 요청</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> SetWorkDirAsync(SetWorkDirRequest req)
	{
		if (!Directory.Exists(req.Path))
			return Task.FromResult(Fail($"Directory not found: {req.Path}"));
		_workDir = req.Path;
		return Task.FromResult(Ok());
	}

	/// <summary>
	/// 디렉터리의 내용을 나열하여 모든 파일 및 하위 디렉터리에 대한 메타데이터를 반환합니다.
	/// </summary>
	/// <param name="req">디렉터리 경로를 포함하는 요청 (비어 있음은 WorkDir을 의미함)</param>
	/// <returns>메타데이터가 있는 파일 목록을 포함하는 응답</returns>
	public Task<FileListResponse> ListFilesAsync(ListFilesRequest req)
	{
		var path    = Resolve(req.Path);
		var entries = new List<FileEntry>();

		foreach (var dir in Directory.GetDirectories(path))
		{
			var di = new DirectoryInfo(dir);
			entries.Add(new FileEntry
			{
				Name = di.Name, IsDir = true,
				ModifiedUnix = new DateTimeOffset(di.LastWriteTimeUtc).ToUnixTimeSeconds()
			});
		}
		foreach (var file in Directory.GetFiles(path))
		{
			var fi = new FileInfo(file);
			entries.Add(new FileEntry
			{
				Name         = fi.Name,
				Size         = fi.Length,
				IsDir        = false,
				ModifiedUnix = new DateTimeOffset(fi.LastWriteTimeUtc).ToUnixTimeSeconds()
			});
		}
		return Task.FromResult(new FileListResponse { Path = path, Files = entries });
	}

	/// <summary>
	/// 파일을 읽고 내용을 Base64 인코딩 문자열로 반환합니다.
	/// </summary>
	/// <param name="req">파일 경로를 포함하는 요청</param>
	/// <returns>Base64 인코딩 파일 데이터를 포함하는 응답</returns>
	public async Task<FileDataResponse> GetFileAsync(GetFileRequest req)
	{
		var path  = Resolve(req.Path);
		var bytes = await File.ReadAllBytesAsync(path);
		return new FileDataResponse { Path = req.Path, DataBase64 = Convert.ToBase64String(bytes) };
	}

	/// <summary>
	/// Base64 인코딩 내용으로 파일을 업로드합니다. 파일이 ZIP 아카이브인 경우 자동으로 추출됩니다.
	/// </summary>
	/// <param name="req">파일명, Base64 인코딩 데이터 및 덮어쓰기 플래그를 포함하는 요청</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public async Task<ResultResponse> PutFileAsync(PutFileRequest req)
	{
		// 파일명만 추출 (directory traversal 방지)
		var fileName = Path.GetFileName(req.Path);
		if (string.IsNullOrWhiteSpace(fileName))
			return Fail("Invalid file name");

		var bytes = Convert.FromBase64String(req.DataBase64);
		if (bytes.Length > _cfg.MaxFileSizeBytes)
			return Fail($"File exceeds max size ({_cfg.MaxFileSizeBytes} bytes)");

		var patchRoot = PatchRoot;
		Directory.CreateDirectory(patchRoot);
		var destPath = Path.Combine(patchRoot, fileName);

		if (File.Exists(destPath) && !req.Overwrite)
			return Fail("File already exists");

		await File.WriteAllBytesAsync(destPath, bytes);

		// ─── zip 파일: 압축 해제 후 원본 삭제 ────────────────
		if (destPath.EndsWith(".zip", StringComparison.OrdinalIgnoreCase))
		{
			try
			{
				ZipFile.ExtractToDirectory(destPath, patchRoot, overwriteFiles: true);
				File.Delete(destPath);
			}
			catch (Exception ex)
			{
				return Fail($"Unzip failed: {ex.Message}");
			}
		}

		return Ok();
	}

	/// <summary>
	/// 파일 또는 디렉터리를 삭제합니다.
	/// </summary>
	/// <param name="req">삭제할 경로를 포함하는 요청</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> DeleteFileAsync(DeleteFileRequest req)
	{
		var path = Resolve(req.Path);
		if (File.Exists(path))
			File.Delete(path);
		else if (Directory.Exists(path))
			Directory.Delete(path, recursive: true);
		else
			return Task.FromResult(Fail("Path not found"));
		return Task.FromResult(Ok());
	}

	/// <summary>
	/// 파일 또는 디렉터리를 이동하거나 이름을 바꿉니다.
	/// </summary>
	/// <param name="req">원본 및 대상 경로를 포함하는 요청</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> MoveFileAsync(MoveFileRequest req)
	{
		var src  = Resolve(req.SourcePath);
		var dest = Resolve(req.DestPath);
		if (File.Exists(src))
			File.Move(src, dest, overwrite: true);
		else if (Directory.Exists(src))
			Directory.Move(src, dest);
		else
			return Task.FromResult(Fail("Source not found"));
		return Task.FromResult(Ok());
	}

	/// <summary>
	/// 지정된 경로에 디렉터리를 만듭니다.
	/// </summary>
	/// <param name="req">생성할 디렉터리 경로를 포함하는 요청</param>
	/// <returns>성공을 나타내는 결과 응답</returns>
	public Task<ResultResponse> MakeDirAsync(MakeDirRequest req)
	{
		Directory.CreateDirectory(Resolve(req.Path));
		return Task.FromResult(Ok());
	}

	/// <summary>
	/// 파일의 SHA-256 해시를 계산하여 반환합니다.
	/// </summary>
	/// <param name="req">파일 경로를 포함하는 요청</param>
	/// <returns>16진수 인코딩 SHA-256 해시를 포함하는 응답</returns>
	public async Task<FileHashResponse> GetFileHashAsync(GetFileHashRequest req)
	{
		var path  = Resolve(req.Path);
		var bytes = await File.ReadAllBytesAsync(path);
		var hash  = Convert.ToHexString(System.Security.Cryptography.SHA256.HashData(bytes)).ToLowerInvariant();
		return new FileHashResponse { Path = req.Path, Hash = hash };
	}

	/// <summary>
	/// 로컬 파일 해시를 원격 해시와 비교하여 누락되고 변경되고 추가된 파일을 식별합니다.
	/// </summary>
	/// <param name="req">경로 및 원격 파일 해시를 포함하는 요청</param>
	/// <returns>누락된 파일, 변경된 파일 및 추가 파일 목록을 포함하는 응답</returns>
	public async Task<SyncDiffResponse> SyncDirAsync(SyncDirRequest req)
	{
		var basePath = Resolve(req.Path);
		var remote   = new Dictionary<string, string>();

		foreach (var file in Directory.EnumerateFiles(basePath, "*", SearchOption.AllDirectories))
		{
			var rel   = Path.GetRelativePath(basePath, file).Replace('\\', '/');
			var bytes = await File.ReadAllBytesAsync(file);
			remote[rel] = Convert.ToHexString(System.Security.Cryptography.SHA256.HashData(bytes)).ToLowerInvariant();
		}

		var localMap = req.LocalFiles.ToDictionary(f => f.RelativePath, f => f.Hash);
		var missing  = localMap.Keys.Where(k => !remote.ContainsKey(k)).ToList();
		var changed  = localMap.Keys.Where(k => remote.TryGetValue(k, out var h) && h != localMap[k]).ToList();
		var extra    = remote.Keys.Where(k => !localMap.ContainsKey(k)).ToList();

		return new SyncDiffResponse { Path = req.Path, Missing = missing, Changed = changed, Extra = extra };
	}

	/// <summary>
	/// 지정된 디렉터리의 ZIP 백업을 만듭니다.
	/// </summary>
	/// <param name="req">원본 디렉터리 및 출력 ZIP 경로를 포함하는 요청</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> BackupDirAsync(BackupDirRequest req)
	{
		var src  = Resolve(req.SourcePath);
		var dest = Resolve(req.OutputPath);
		if (!Directory.Exists(src))
			return Task.FromResult(Fail("Source directory not found"));
		ZipFile.CreateFromDirectory(src, dest, CompressionLevel.Optimal, includeBaseDirectory: false);
		return Task.FromResult(Ok());
	}

	/// <summary>
	/// 디렉터리를 ZIP 또는 TAR.GZ 형식으로 압축합니다.
	/// </summary>
	/// <param name="req">원본 디렉터리, 출력 경로 및 형식 (zip 또는 targz)을 포함하는 요청</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> CompressAsync(CompressRequest req)
	{
		var src  = Resolve(req.SourcePath);
		var dest = Resolve(req.OutputPath);
		if ("targz" == req.Format)
		{
			using var fs  = File.Create(dest);
			using var gz  = new GZipStream(fs, CompressionLevel.Optimal);
			System.Formats.Tar.TarFile.CreateFromDirectory(src, gz, includeBaseDirectory: false);
		}
		else
		{
			ZipFile.CreateFromDirectory(src, dest, CompressionLevel.Optimal, includeBaseDirectory: false);
		}
		return Task.FromResult(Ok());
	}

	/// <summary>
	/// ZIP 또는 TAR.GZ 아카이브를 지정된 출력 디렉터리로 압축 해제합니다.
	/// </summary>
	/// <param name="req">원본 아카이브 경로 및 출력 디렉터리를 포함하는 요청</param>
	/// <returns>성공 또는 실패를 나타내는 결과 응답</returns>
	public Task<ResultResponse> DecompressAsync(DecompressRequest req)
	{
		var src  = Resolve(req.SourcePath);
		var dest = Resolve(req.OutputPath);
		if (src.EndsWith(".tar.gz", StringComparison.OrdinalIgnoreCase) ||
			src.EndsWith(".tgz",    StringComparison.OrdinalIgnoreCase))
		{
			using var fs = File.OpenRead(src);
			using var gz = new GZipStream(fs, CompressionMode.Decompress);
			System.Formats.Tar.TarFile.ExtractToDirectory(gz, dest, overwriteFiles: true);
		}
		else
		{
			ZipFile.ExtractToDirectory(src, dest, overwriteFiles: true);
		}
		return Task.FromResult(Ok());
	}

	// @brief 상대 경로를 WorkDir 기준 절대 경로로 변환
	private string Resolve(string path) =>
		Path.IsPathRooted(path) ? path : Path.GetFullPath(Path.Combine(_workDir, path));

	private static ResultResponse Ok()                 => new() { Success = true };
	private static ResultResponse Fail(string message) => new() { Success = false, Message = message, Code = 400 };
}
