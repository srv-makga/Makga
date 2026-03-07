using System.Data;
using Login.Models;
using MySqlConnector;

namespace Login.Repository;

/// <summary>
/// 계정 DB 접근 레이어. MySQL Stored Procedure를 통해 모든 쿼리를 실행한다.
/// Stored Procedure 목록은 tables/sql/account_db.sql 을 참조한다.
/// </summary>
public sealed class AccountRepository(MySqlDataSource dataSource)
{
    /// <summary>
    /// 이메일·패스워드 방식으로 신규 계정을 등록한다.
    /// SP: sp_register_self (p_email, p_password_hash → p_account_uid OUT, p_error_code OUT)
    /// </summary>
    /// <returns>
    /// (AccountUid, ErrorCode):
    ///   ErrorCode == 0  → 성공, AccountUid에 새 UID 반환<br/>
    ///   ErrorCode == -1 → 이메일 중복
    /// </returns>
    public async Task<(long AccountUid, int ErrorCode)> RegisterSelfAsync(
        string email, string passwordHash)
    {
        await using var conn = await dataSource.OpenConnectionAsync();
        await using var cmd  = conn.CreateCommand();
        cmd.CommandText = "sp_register_self";
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("p_email",         email);
        cmd.Parameters.AddWithValue("p_password_hash", passwordHash);

        // OUT 파라미터: SP 실행 후 채워진 값을 읽는다
        var uidParam = new MySqlParameter("p_account_uid", MySqlDbType.UInt64)
            { Direction = ParameterDirection.Output };
        var errParam = new MySqlParameter("p_error_code", MySqlDbType.Byte)
            { Direction = ParameterDirection.Output };
        cmd.Parameters.Add(uidParam);
        cmd.Parameters.Add(errParam);

        await cmd.ExecuteNonQueryAsync();
        return (Convert.ToInt64(uidParam.Value), Convert.ToInt32(errParam.Value));
    }

    /// <summary>
    /// 이메일로 자체 로그인용 계정 정보를 조회한다. 패스워드 해시 비교에 사용.
    /// SP: sp_login_self (p_email → result set: account_uid, password_hash, status)
    /// </summary>
    /// <returns>계정이 존재하면 <see cref="SelfLoginRow"/>, 없으면 null</returns>
    public async Task<SelfLoginRow?> GetSelfLoginAsync(string email)
    {
        await using var conn   = await dataSource.OpenConnectionAsync();
        await using var cmd    = conn.CreateCommand();
        cmd.CommandText = "sp_login_self";
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("p_email", email);

        await using var reader = await cmd.ExecuteReaderAsync();
        if (!await reader.ReadAsync()) return null;

        return new SelfLoginRow(
            reader.GetInt64("account_uid"),
            reader.GetString("password_hash"),
            reader.GetString("status"));
    }

    /// <summary>
    /// OAuth 계정을 조회하거나 없으면 신규 생성(Upsert)한다.
    /// (loginType, oauthUid) 쌍이 고유 식별자이며, 이메일은 선택 저장된다.
    /// SP: sp_upsert_oauth (p_login_type, p_oauth_uid, p_email → result set: account_uid, status)
    /// </summary>
    /// <param name="loginType">플랫폼 식별자 ("google" | "apple")</param>
    /// <param name="oauthUid">플랫폼 고유 사용자 ID (Google/Apple의 sub 클레임)</param>
    /// <param name="email">플랫폼 제공 이메일 (null 허용)</param>
    public async Task<AccountRow?> UpsertOAuthAsync(
        string loginType, string oauthUid, string? email)
    {
        await using var conn   = await dataSource.OpenConnectionAsync();
        await using var cmd    = conn.CreateCommand();
        cmd.CommandText = "sp_upsert_oauth";
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("p_login_type", loginType);
        cmd.Parameters.AddWithValue("p_oauth_uid",  oauthUid);
        // email이 null이면 DBNull.Value로 변환하여 SQL NULL로 전달
        cmd.Parameters.AddWithValue("p_email",      (object?)email ?? DBNull.Value);

        await using var reader = await cmd.ExecuteReaderAsync();
        if (!await reader.ReadAsync()) return null;

        return new AccountRow(
            reader.GetInt64("account_uid"),
            reader.GetString("status"));
    }

    /// <summary>
    /// 게스트 계정을 deviceId로 조회하거나, 없으면 신규 생성(Upsert)한다.
    /// SP: sp_login_guest (p_device_id → result set: account_uid, status)
    /// </summary>
    /// <param name="deviceId">클라이언트가 생성한 UUID (기기 고유 식별자)</param>
    public async Task<AccountRow?> LoginGuestAsync(string deviceId)
    {
        await using var conn   = await dataSource.OpenConnectionAsync();
        await using var cmd    = conn.CreateCommand();
        cmd.CommandText = "sp_login_guest";
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("p_device_id", deviceId);

        await using var reader = await cmd.ExecuteReaderAsync();
        if (!await reader.ReadAsync()) return null;

        return new AccountRow(
            reader.GetInt64("account_uid"),
            reader.GetString("status"));
    }

    /// <summary>
    /// 테스트 계정을 testKey로 조회한다. (Auth:EnableTestLogin = true 환경 전용)
    /// SP: sp_login_test (p_test_key → result set: account_uid, status)
    /// </summary>
    public async Task<AccountRow?> LoginTestAsync(string testKey)
    {
        await using var conn   = await dataSource.OpenConnectionAsync();
        await using var cmd    = conn.CreateCommand();
        cmd.CommandText = "sp_login_test";
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("p_test_key", testKey);

        await using var reader = await cmd.ExecuteReaderAsync();
        if (!await reader.ReadAsync()) return null;

        return new AccountRow(
            reader.GetInt64("account_uid"),
            reader.GetString("status"));
    }

    /// <summary>
    /// 계정의 last_login 시각을 현재 시각으로 갱신한다. 로그인 성공 시 항상 호출된다.
    /// SP: sp_update_last_login (p_account_uid)
    /// </summary>
    public async Task UpdateLastLoginAsync(long accountUid)
    {
        await using var conn = await dataSource.OpenConnectionAsync();
        await using var cmd  = conn.CreateCommand();
        cmd.CommandText = "sp_update_last_login";
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("p_account_uid", accountUid);
        await cmd.ExecuteNonQueryAsync();
    }

    /// <summary>
    /// 내부 서버 연결용 세션 토큰을 account_session 테이블에 저장(Upsert)한다.
    /// 동일 account_uid의 기존 토큰은 덮어쓴다 (계정당 1개의 활성 세션 토큰 유지).
    /// SP: sp_upsert_session_token (p_account_uid, p_session_token, p_login_type, p_expires_at)
    /// </summary>
    /// <param name="accountUid">내부 계정 고유 ID</param>
    /// <param name="sessionToken">생성된 세션 토큰 (32자 hex)</param>
    /// <param name="loginType">로그인 방식 (self | google | apple | guest | test)</param>
    /// <param name="expiresAt">토큰 만료 시각 (UTC)</param>
    public async Task UpsertSessionTokenAsync(
        long accountUid, string sessionToken, string loginType, DateTime expiresAt)
    {
        await using var conn = await dataSource.OpenConnectionAsync();
        await using var cmd  = conn.CreateCommand();
        cmd.CommandText = "sp_upsert_session_token";
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("p_account_uid",   accountUid);
        cmd.Parameters.AddWithValue("p_session_token", sessionToken);
        cmd.Parameters.AddWithValue("p_login_type",    loginType);
        cmd.Parameters.AddWithValue("p_expires_at",    expiresAt);
        await cmd.ExecuteNonQueryAsync();
    }
}

