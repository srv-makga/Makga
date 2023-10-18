#pragma once

#include "db_statement.h"

namespace core {
namespace db {
struct DBConnectorInfo
{
	std::string host;
	uint16_t port = 0;
	std::string id;
	std::string pw;
	std::string db;
	std::string encoding;
};

class DBConnector
{
public:
	DBConnector();
	virtual ~DBConnector() {}

	bool Connect(const std::string& _host, uint16_t _port, const std::string& _id, const std::string& _pw, const std::string& _db, const std::string& _encoding);

	virtual bool Disconnect() = 0;
	virtual bool IsConnected() const = 0;
	virtual bool Reconnect() = 0;
	virtual Statement* CreateStatement(int32_t _key, const char* _query) = 0;

protected:
	virtual bool Connect() = 0;

protected:
	DBConnectorInfo m_info;
	bool m_is_connected;
};
} // namespace db
} // namespace core