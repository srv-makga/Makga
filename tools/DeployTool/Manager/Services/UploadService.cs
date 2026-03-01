using Microsoft.Extensions.Options;

namespace DeployTool.Manager.Services;

// @brief Manager 의 Upload 디렉토리를 관리한다.
//        파일 저장·목록·삭제만 담당하며, 패치 순서 관리는 Patch.razor 에서 수행한다.
public class UploadService
{
	public string UploadDir { get; }

	public UploadService(IOptions<ManagerConfig> cfg, IWebHostEnvironment env)
	{
		var configured = cfg.Value.UploadDir;
		UploadDir = string.IsNullOrWhiteSpace(configured)
			? Path.Combine(env.ContentRootPath, "Upload")
			: Path.IsPathRooted(configured)
				? configured
				: Path.Combine(env.ContentRootPath, configured);

		Directory.CreateDirectory(UploadDir);
	}

	// @brief Upload 디렉토리의 파일 목록을 업로드(생성) 시간 순으로 반환한다.
	public IReadOnlyList<FileInfo> GetFiles() =>
		new DirectoryInfo(UploadDir)
			.GetFiles()
			.OrderBy(f => f.CreationTimeUtc)
			.ToList();

	// @brief 스트림 데이터를 Upload 디렉토리에 저장한다.
	public async Task SaveAsync(string fileName, Stream stream)
	{
		var safe = Path.GetFileName(fileName); // directory traversal 방지
		var path = Path.Combine(UploadDir, safe);
		await using var fs = File.Create(path);
		await stream.CopyToAsync(fs);
	}

	// @brief Upload 디렉토리에서 파일을 삭제한다.
	public void Delete(string fileName)
	{
		var path = Path.Combine(UploadDir, Path.GetFileName(fileName));
		if (File.Exists(path))
			File.Delete(path);
	}

	public string FilePath(string fileName) =>
		Path.Combine(UploadDir, Path.GetFileName(fileName));
}
