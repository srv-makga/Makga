#pragma once

#include "db_connector.h"
#include <mariadb/conncpp.hpp>

namespace core {
namespace db {
namespace maria {
class MariaConnector : public DBConnector
{
public:
	MariaConnector();
	virtual ~MariaConnector();

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
} // namespace maria
} // namespace db
} // namespace core