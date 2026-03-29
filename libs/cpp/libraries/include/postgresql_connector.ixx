module;

#include <memory>
#include <string>

export module makga.lib.database.postgresql;

import makga.lib.database.connector;

export namespace makga::lib::database {

// libpq 기반 PostgreSQL 커넥터 (PIMPL — libpq-fe.h는 .cpp에만 포함)
// 빌드 전 설정:
//   AdditionalIncludeDirectories : $(PGSQL_HOME)\include
//   AdditionalDependencies       : libpq.lib
export class PostgreSqlConnector : public IDbConnector
{
public:
	PostgreSqlConnector();
	~PostgreSqlConnector() override;

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
