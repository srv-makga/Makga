#pragma once

#include "db_connector.h"
#include "mysql/jdbc.h"

namespace core {
namespace db {
namespace mysql {
class MysqlConnector : public DBConnector
{
public:
	MysqlConnector();
	virtual ~MysqlConnector();

	void Initialize();

	bool Disconnect() override;
	bool IsConnected() const override;
	bool Reconnect() override;

	Statement* CreateStatement(int32_t _key, const char* _query) override;

protected:
	bool Connect() override;

private:
	sql::Driver* m_driver;
	std::unique_ptr<sql::Connection> m_conn;
};
} // namespace mysql
} // namespace db
} // namespace core