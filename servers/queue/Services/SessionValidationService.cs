using MySql.Data.MySqlClient;

namespace CSharp.Queue.Services;

public interface ISessionValidationService
{
    Task<long?> ValidateSessionTokenAsync(string sessionToken);
}

public class SessionValidationService : ISessionValidationService
{
    private readonly string _connectionString;
    private readonly ILogger<SessionValidationService> _logger;

    public SessionValidationService(IConfiguration configuration, ILogger<SessionValidationService> logger)
    {
        _connectionString = configuration.GetConnectionString("MySQL") 
            ?? throw new InvalidOperationException("MySQL connection string not configured");
        _logger = logger;
    }

    public async Task<long?> ValidateSessionTokenAsync(string sessionToken)
    {
        try
        {
            using var connection = new MySqlConnection(_connectionString);
            await connection.OpenAsync();

            const string query = @"
                SELECT account_uid 
                FROM user_sessions 
                WHERE session_token = @SessionToken 
                  AND expires_at > @Now 
                  AND is_active = 1
                LIMIT 1";

            using var command = new MySqlCommand(query, connection);
            command.Parameters.AddWithValue("@SessionToken", sessionToken);
            command.Parameters.AddWithValue("@Now", DateTime.UtcNow);

            var result = await command.ExecuteScalarAsync();
            
            if (result != null && long.TryParse(result.ToString(), out long accountUid))
            {
                return accountUid;
            }

            return null;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to validate session token");
            return null;
        }
    }
}
