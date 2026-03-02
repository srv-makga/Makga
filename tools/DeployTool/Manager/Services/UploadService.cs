using Microsoft.Extensions.Options;

namespace DeployTool.Manager.Services;

/// <summary>
/// Manages patch file uploads to the upload directory.
/// Provides listing, saving, and deletion of uploaded files.
/// </summary>
public class UploadService
{
	/// <summary>
	/// Gets the upload directory path.
	/// </summary>
	public string UploadDir { get; }

	/// <summary>
	/// Initializes a new instance of the UploadService class.
	/// Creates the upload directory if it doesn't exist.
	/// </summary>
	/// <param name="cfg">Manager configuration</param>
	/// <param name="env">Web host environment for base path resolution</param>
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

	/// <summary>
	/// Gets a list of uploaded files sorted by creation time.
	/// </summary>
	/// <returns>Read-only list of uploaded files</returns>
	public IReadOnlyList<FileInfo> GetFiles() =>
		new DirectoryInfo(UploadDir)
			.GetFiles()
			.OrderBy(f => f.CreationTimeUtc)
			.ToList();

	/// <summary>
	/// Saves an uploaded file to the upload directory.
	/// </summary>
	/// <param name="fileName">Name of the file</param>
	/// <param name="stream">File content stream</param>
	/// <returns>Completion task</returns>
	public async Task SaveAsync(string fileName, Stream stream)
	{
		var safe = Path.GetFileName(fileName);
		var path = Path.Combine(UploadDir, safe);
		await using var fs = File.Create(path);
		await stream.CopyToAsync(fs);
	}

	/// <summary>
	/// Deletes a file from the upload directory.
	/// </summary>
	/// <param name="fileName">Name of the file to delete</param>
	public void Delete(string fileName)
	{
		var path = Path.Combine(UploadDir, Path.GetFileName(fileName));
		if (File.Exists(path))
			File.Delete(path);
	}

	/// <summary>
	/// Gets the full file path for an uploaded file.
	/// </summary>
	/// <param name="fileName">File name</param>
	/// <returns>Complete file path</returns>
	public string FilePath(string fileName) =>
		Path.Combine(UploadDir, Path.GetFileName(fileName));
}
