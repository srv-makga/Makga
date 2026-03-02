using DeployTool.Manager.Services;

namespace DeployTool.Manager.Components.Pages;

/// <summary>Code-behind for Settings page. Manages Manager configuration parameters.</summary>
public partial class Settings
{
	private int _retryIntervalSec;
	private int _heartbeatIntervalSec;
	private bool _saving;
	private string _message = "";
	private bool _isError;

	/// <summary>Initializes settings from current configuration.</summary>
	protected override void OnInitialized()
	{
		var cfg = SettingsSvc.Current;
		_retryIntervalSec     = cfg.RetryIntervalSec;
		_heartbeatIntervalSec = cfg.HeartbeatIntervalSec;
	}

	/// <summary>Saves settings changes to appsettings.json.</summary>
	private async Task SaveAsync()
	{
		_saving  = true;
		_message = "";
		try
		{
			await SettingsSvc.SaveAsync(_retryIntervalSec, _heartbeatIntervalSec);
			_isError = false;
			_message = L["Settings_SaveDone"];
		}
		catch (Exception ex)
		{
			_isError = true;
			_message = ex.Message;
		}
		finally
		{
			_saving = false;
		}
	}
}
