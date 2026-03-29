module;

#include <memory>
#include <string>

export module makga.lib.database.mysql;

import makga.lib.database.connector;

export namespace makga::lib::database {

// MySQL C API 기반 커넥터 (PIMPL — mysql.h는 .cpp에만 포함)
// 빌드 전 설정:
//   AdditionalIncludeDirectories : $(MYSQL_HOME)\include
//   AdditionalDependencies       : libmysql.lib
export class MySqlConnector : public IDbConnector
{
public:
	MySqlConnector();
	~MySqlConnector() override;

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
