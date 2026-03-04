using System.Security.Cryptography;
using DeployTool.Common.Packets;
using DeployTool.Manager.Services;
using Microsoft.AspNetCore.Components.Forms;

namespace DeployTool.Manager.Components.Pages;

/// <summary>Code-behind for Patch deployment page. Manages file uploads and patch distribution.</summary>
public partial class Patch
{
	private List<FileEntry> _files = new();
	private HashSet<string> _checkedFiles = new();
	private List<PendingUpload> _pendingUploads = new();
	private bool _uploading;
	private string _inputKey = Guid.NewGuid().ToString();
	private List<AgentClient> _agents = new();
	private HashSet<string> _selectedAgents = new();
	private HashSet<string> _activeGroups = new();
	private HashSet<string> _activeTags = new();
	private List<(string Agent, string Status)> _progress = new();
	private bool _patching;

	/// <summary>Local file entry for patch list.</summary>
	private record FileEntry(string Name, long Size);
	/// <summary>Pending upload entry with file info.</summary>
	private record PendingUpload(string Name, IBrowserFile File);

	/// <summary>Maximum file size allowed (500MB).</summary>
	private const long UploadLimit = 500L * 1024 * 1024;

	/// <summary>Gets filtered agent list based on selected groups and tags.</summary>
	private bool IsAgentFilterMatch(AgentClient agent)
	{
		if (_activeGroups.Count > 0)
		{
			var group = agent.Info.Group ?? string.Empty;
			if (!_activeGroups.Contains(group)) return false;
		}

		if (_activeTags.Count > 0)
		{
			var tags = agent.Info.Tags ?? Enumerable.Empty<string>();
			if (!tags.Any(t => _activeTags.Contains(t))) return false;
		}

		return true;
	}

	/// <summary>Initializes the page and loads file list.</summary>
	protected override void OnInitialized()
	{
		_agents = Registry.All.ToList();
		RefreshFileList();
	}

	/// <summary>Refreshes the file list from disk, preserving order and selection.</summary>
	private void RefreshFileList()
	{
		var diskFiles = Uploads.GetFiles();

		var diskNames = diskFiles.Select(f => f.Name).ToHashSet();
		_files.RemoveAll(e => !diskNames.Contains(e.Name));
		_checkedFiles.RemoveWhere(n => !diskNames.Contains(n));

		var existing = _files.Select(e => e.Name).ToHashSet();
		foreach (var fi in diskFiles.Where(f => !existing.Contains(f.Name)))
		{
			_files.Add(new FileEntry(fi.Name, fi.Length));
			_checkedFiles.Add(fi.Name);
		}
	}

	/// <summary>Handles file selection from input element.</summary>
	private async Task HandleFilesSelected(InputFileChangeEventArgs e)
	{
		foreach (var file in e.GetMultipleFiles(50))
			QueuePendingUpload(file);

		await InvokeAsync(StateHasChanged);
	}

	/// <summary>Uploads pending files to the upload directory.</summary>
	private async Task UploadPendingAsync()
	{
		if (!_pendingUploads.Any()) return;

		_uploading = true;
		await InvokeAsync(StateHasChanged);
		try
		{
			foreach (var pending in _pendingUploads)
			{
				await using var stream = pending.File.OpenReadStream(UploadLimit);
				await Uploads.SaveAsync(pending.Name, stream);
			}
			_pendingUploads.Clear();
			RefreshFileList();
			ResetFileInput();
		}
		catch (Exception ex)
		{
			Console.Error.WriteLine($"[Patch] Upload error: {ex.Message}");
		}
		finally
		{
			_uploading = false;
			await InvokeAsync(StateHasChanged);
		}
	}

	/// <summary>Toggles selection state for all files.</summary>
	private void ToggleAllFiles(bool selected)
	{
		if (selected) foreach (var f in _files) _checkedFiles.Add(f.Name);
		else          _checkedFiles.Clear();
		StateHasChanged();
	}

	/// <summary>Toggles selection state for a specific file.</summary>
	private void ToggleFile(string name, bool selected)
	{
		if (selected) _checkedFiles.Add(name);
		else          _checkedFiles.Remove(name);
		StateHasChanged();
	}

	/// <summary>Deletes a file from the upload directory.</summary>
	private void DeleteFile(int idx)
	{
		if (idx < 0 || idx >= _files.Count) return;
		var name = _files[idx].Name;
		Uploads.Delete(name);
		_checkedFiles.Remove(name);
		_files.RemoveAt(idx);
		StateHasChanged();
	}

	/// <summary>Moves a file up in the patch order.</summary>
	private void MoveUp(int idx)
	{
		if (idx <= 0) return;
		(_files[idx], _files[idx - 1]) = (_files[idx - 1], _files[idx]);
		StateHasChanged();
	}

	/// <summary>Moves a file down in the patch order.</summary>
	private void MoveDown(int idx)
	{
		if (idx >= _files.Count - 1) return;
		(_files[idx], _files[idx + 1]) = (_files[idx + 1], _files[idx]);
		StateHasChanged();
	}

	/// <summary>Toggles selection state for a specific agent.</summary>
	private void ToggleAgent(string name, bool selected)
	{
		if (selected) _selectedAgents.Add(name);
		else          _selectedAgents.Remove(name);
		StateHasChanged();
	}

	/// <summary>Toggles filtering by agent group.</summary>
	private void ToggleAgentGroup(string group)
	{
		if (!_activeGroups.Add(group))
			_activeGroups.Remove(group);

		PruneSelectedAgents();
		StateHasChanged();
	}

	/// <summary>Toggles filtering by agent tag.</summary>
	private void ToggleAgentTag(string tag)
	{
		if (!_activeTags.Add(tag))
			_activeTags.Remove(tag);

		PruneSelectedAgents();
		StateHasChanged();
	}

	/// <summary>Checks if a group filter is active.</summary>
	private bool IsGroupFilterActive(string group) => _activeGroups.Contains(group);

	/// <summary>Checks if a tag filter is active.</summary>
	private bool IsTagFilterActive(string tag) => _activeTags.Contains(tag);

	/// <summary>Removes selected agents that don't match current filters.</summary>
	private void PruneSelectedAgents()
	{
		if (_activeGroups.Count == 0 && _activeTags.Count == 0) return;
		if (_selectedAgents.Count == 0) return;

		var allowed = new HashSet<string>(
			_agents.Where(IsAgentFilterMatch).Select(a => a.Info.Name),
			StringComparer.OrdinalIgnoreCase);

		foreach (var agentName in _selectedAgents.ToList())
		{
			if (!allowed.Contains(agentName))
				_selectedAgents.Remove(agentName);
		}
	}

	/// <summary>Queues a file for upload if not already queued.</summary>
	private void QueuePendingUpload(IBrowserFile file)
	{
		if (_pendingUploads.Any(p => p.Name == file.Name)) return;
		_pendingUploads.Add(new PendingUpload(file.Name, file));
	}

	/// <summary>Resets the file input element key.</summary>
	private void ResetFileInput() => _inputKey = Guid.NewGuid().ToString();

	/// <summary>Starts patch deployment to selected agents.</summary>
	private async Task StartPatchAsync()
	{
		_patching = true;
		_progress = _selectedAgents.Select(a => (a, "대기 중")).ToList();

		var filesToPatch = _files.Where(f => _checkedFiles.Contains(f.Name)).ToList();

		for (var ai = 0; ai < _progress.Count; ai++)
		{
			var agentName = _progress[ai].Agent;
			var client    = Registry.Get(agentName);

			if (null == client || !client.IsConnected)
			{
				_progress[ai] = (agentName, "offline");
				await InvokeAsync(StateHasChanged);
				continue;
			}

			var failed = false;
			for (var fi = 0; fi < filesToPatch.Count; fi++)
			{
				var entry = filesToPatch[fi];
				_progress[ai] = (agentName, $"{entry.Name} ({fi + 1}/{filesToPatch.Count})");
				await InvokeAsync(StateHasChanged);

				byte[] bytes;
				try
				{
					bytes = await File.ReadAllBytesAsync(Uploads.FilePath(entry.Name));
				}
				catch (Exception ex)
				{
					_progress[ai] = (agentName, $"fail:파일 읽기 오류 - {ex.Message}");
					await InvokeAsync(StateHasChanged);
					failed = true;
					break;
				}

				var sha256 = Convert.ToHexString(SHA256.HashData(bytes)).ToLowerInvariant();
				var result = await client.PutFileChunkedAsync(
					entry.Name, bytes, sha256, overwrite: true,
					onChunk: (current, total) =>
					{
						_progress[ai] = (agentName, total > 1
							? $"{entry.Name} ({fi + 1}/{filesToPatch.Count}) [{current}/{total}청크]"
							: $"{entry.Name} ({fi + 1}/{filesToPatch.Count})");
						_ = InvokeAsync(StateHasChanged);
					});

				if (result?.Success == false)
				{
					_progress[ai] = (agentName, $"fail:{result.Message}");
					await InvokeAsync(StateHasChanged);
					failed = true;
					break;
				}
			}

			if (!failed)
			{
				_progress[ai] = (agentName, "done");
				await InvokeAsync(StateHasChanged);
			}
		}

		_patching = false;
		await InvokeAsync(StateHasChanged);
	}

	/// <summary>Formats bytes to human-readable string.</summary>
	private static string FormatBytes(long bytes)
	{
		if (bytes >= 1_073_741_824) return $"{bytes / 1_073_741_824.0:0.0} GB";
		if (bytes >= 1_048_576)     return $"{bytes / 1_048_576.0:0.0} MB";
		if (bytes >= 1_024)         return $"{bytes / 1_024.0:0.0} KB";
		return $"{bytes} B";
	}

	/// <summary>Gets whether patch deployment can be started (has files and agents selected).</summary>
	private bool CanPatch => !_patching && _checkedFiles.Count > 0 && _selectedAgents.Count > 0;

	/// <summary>Gets whether there are pending uploads waiting to be uploaded.</summary>
	private bool HasPendingUploads => _pendingUploads.Count > 0;

	/// <summary>Gets whether all files are currently checked for deployment.</summary>
	private bool AllFilesChecked => _files.Count > 0 && _checkedFiles.Count == _files.Count;

	/// <summary>Gets list of unique agent groups for filtering.</summary>
	private IEnumerable<string> AgentGroups => _agents
		.Where(a => !string.IsNullOrEmpty(a.Info.Group))
		.Select(a => a.Info.Group!)
		.Distinct()
		.Order();

	/// <summary>Gets list of unique agent tags for filtering.</summary>
	private IEnumerable<string> AgentTags => _agents
		.SelectMany(a => a.Info.Tags ?? Enumerable.Empty<string>())
		.Distinct()
		.Order();

	/// <summary>Clears all active group and tag filters.</summary>
	private void ClearFilters()
	{
		_activeGroups.Clear();
		_activeTags.Clear();
		_selectedAgents.Clear();
		StateHasChanged();
	}

	/// <summary>Disposes resources.</summary>
	public void Dispose() { }
}
