using System.IO.Compression;
using System.Security.Cryptography;
using DeployTool.Common.Packets;
using Microsoft.Extensions.Options;

namespace DeployTool.Agent.Services;

public class FileService
{
	private readonly AgentConfig _cfg;
	private string _workDir;

	public FileService(IOptions<AgentConfig> cfg)
	{
		_cfg     = cfg.Value;
		_workDir = _cfg.WorkDir;
	}

	// @brief 패치 파일의 저장 루트. PatchRootDir 이 비어있으면 WorkDir 를 사용
	private string PatchRoot =>
		string.IsNullOrWhiteSpace(_cfg.PatchRootDir) ? _workDir : _cfg.PatchRootDir;

	public Task<ResultResponse> SetWorkDirAsync(SetWorkDirRequest req)
	{
		if (!Directory.Exists(req.Path))
			return Task.FromResult(Fail($"Directory not found: {req.Path}"));
		_workDir = req.Path;
		return Task.FromResult(Ok());
	}

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

	public async Task<FileDataResponse> GetFileAsync(GetFileRequest req)
	{
		var path  = Resolve(req.Path);
		var bytes = await File.ReadAllBytesAsync(path);
		return new FileDataResponse { Path = req.Path, DataBase64 = Convert.ToBase64String(bytes) };
	}

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

	public Task<ResultResponse> MakeDirAsync(MakeDirRequest req)
	{
		Directory.CreateDirectory(Resolve(req.Path));
		return Task.FromResult(Ok());
	}

	public async Task<FileHashResponse> GetFileHashAsync(GetFileHashRequest req)
	{
		var path  = Resolve(req.Path);
		var bytes = await File.ReadAllBytesAsync(path);
		var hash  = Convert.ToHexString(SHA256.HashData(bytes)).ToLowerInvariant();
		return new FileHashResponse { Path = req.Path, Hash = hash };
	}

	public async Task<SyncDiffResponse> SyncDirAsync(SyncDirRequest req)
	{
		var basePath = Resolve(req.Path);
		var remote   = new Dictionary<string, string>();

		foreach (var file in Directory.EnumerateFiles(basePath, "*", SearchOption.AllDirectories))
		{
			var rel   = Path.GetRelativePath(basePath, file).Replace('\\', '/');
			var bytes = await File.ReadAllBytesAsync(file);
			remote[rel] = Convert.ToHexString(SHA256.HashData(bytes)).ToLowerInvariant();
		}

		var localMap = req.LocalFiles.ToDictionary(f => f.RelativePath, f => f.Hash);
		var missing  = localMap.Keys.Where(k => !remote.ContainsKey(k)).ToList();
		var changed  = localMap.Keys.Where(k => remote.TryGetValue(k, out var h) && h != localMap[k]).ToList();
		var extra    = remote.Keys.Where(k => !localMap.ContainsKey(k)).ToList();

		return new SyncDiffResponse { Path = req.Path, Missing = missing, Changed = changed, Extra = extra };
	}

	public Task<ResultResponse> BackupDirAsync(BackupDirRequest req)
	{
		var src  = Resolve(req.SourcePath);
		var dest = Resolve(req.OutputPath);
		if (!Directory.Exists(src))
			return Task.FromResult(Fail("Source directory not found"));
		ZipFile.CreateFromDirectory(src, dest, CompressionLevel.Optimal, includeBaseDirectory: false);
		return Task.FromResult(Ok());
	}

	public Task<ResultResponse> CompressAsync(CompressRequest req)
	{
		var src  = Resolve(req.SourcePath);
		var dest = Resolve(req.OutputPath);
		if ("targz" == req.Format)
		{
			// .NET 8 TarFile API
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
