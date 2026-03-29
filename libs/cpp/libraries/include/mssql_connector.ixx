module;

#include <memory>
#include <string>

export module makga.lib.database.mssql;

import makga.lib.database.connector;

export namespace makga::lib::database {

// ODBC 기반 MSSQL 커넥터 (PIMPL — sql.h/sqlext.h는 .cpp에만 포함)
// 빌드 전 설정:
//   AdditionalDependencies : odbc32.lib  (Windows SDK 기본 제공)
export class MssqlConnector : public IDbConnector
{
public:
	MssqlConnector();
	~MssqlConnector() override;

	bool Connect(const DbConfig& config) override;
	void Disconnect() override;
	bool IsConnected() const override;
	bool Execute(const std::string& query, DbRows& out_rows) override;
	bool ExecuteNonQuery(const std::string& query) override;

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

} // namespace makga::lib::database
