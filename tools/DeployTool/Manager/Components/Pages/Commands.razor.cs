using DeployTool.Common;
using DeployTool.Common.Net;
using DeployTool.Common.Packets;
using DeployTool.Manager.Services;
using Microsoft.AspNetCore.Components;
using Microsoft.AspNetCore.Components.Web;

namespace DeployTool.Manager.Components.Pages;

/// <summary>Code-behind for Agent command execution page. Allows running shell and process commands.</summary>
public partial class Commands
{
	private AgentClient? _client;
	private bool _notFound;
	private string _shellCmd = "";
	private string _shellOut = "";
	private string _execProg = "";
	private string _execArgs = "";
	private bool _execDetach;
	private string _execOut = "";

	/// <summary>Agent name parameter from URL</summary>
	[Parameter] public string Name { get; set; } = "";

	/// <summary>Initializes the page and gets Agent client reference.</summary>
	protected override void OnInitialized()
	{
		_client = Registry.Get(Name);
		if (null == _client)
			_notFound = true;
	}

	/// <summary>Executes shell command on Enter key press.</summary>
	private async Task OnShellKeyDown(KeyboardEventArgs e)
	{
		if (e.Key == "Enter")
			await RunShellAsync();
	}

	/// <summary>Runs a shell command and displays output.</summary>
	private async Task RunShellAsync()
	{
		if (null == _client || string.IsNullOrWhiteSpace(_shellCmd)) return;

		var resp = await _client.SendAsync(PacketId.Shell,
			new ShellRequest { Command = _shellCmd });

		if (resp.HasValue)
		{
			var d = PacketSerializer.Deserialize<ShellOutputResponse>(resp.Value.payload);
			_shellOut = d is null
				? "Error"
				: $"[exit {d.ExitCode}]\n{d.Stdout}{(string.IsNullOrEmpty(d.Stderr) ? "" : "\n[stderr]\n" + d.Stderr)}";
		}
		else
		{
			_shellOut = "Error";
		}
	}

	/// <summary>Executes a process and displays result.</summary>
	private async Task RunExecAsync()
	{
		if (null == _client || string.IsNullOrWhiteSpace(_execProg)) return;

		var argList = _execArgs.Split(' ', StringSplitOptions.RemoveEmptyEntries).ToList();
		var resp = await _client.SendAsync(PacketId.Execute,
			new ExecuteRequest { Program = _execProg, Args = argList, WorkDir = "", Detach = _execDetach });

		if (resp.HasValue)
		{
			var d = PacketSerializer.Deserialize<ResultResponse>(resp.Value.payload);
			_execOut = d is null ? "Error" : $"success={d.Success} code={d.Code} {d.Message}";
		}
		else
		{
			_execOut = "Error";
		}
	}
}
