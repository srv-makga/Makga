#pragma once
#include <mariadb/conncpp.hpp>
#include <map>

#define CLEAR_RESULT_SET(stmt)	if (nullptr != stmt) { do { std::unique_ptr<sql::ResultSet> result_set(stmt->getResultSet()); } while(stmt->getMoreResults()); } 

namespace core {
class DBClientMaria
{
	std::string m_host;
	std::string m_id;
	std::string m_pwd;
	std::string m_db_name;
	int m_port;
	std::string m_charset;

	sql::Driver*								m_driver;
	std::unique_ptr<sql::Connection>			m_conn;
	std::map<int32_t, sql::PreparedStatement*>	m_stmts;
	sql::PreparedStatement*						m_stmt_ping;

public:
	DBClientMaria();
	~DBClientMaria();

public:
	bool	Connect(std::string& host, std::string& id, std::string& pwd, std::string& db_name, int port = 4306, const char* charset = nullptr);
	bool	Connect();
	void	Close();

	bool	Ping();

public:

	bool						RegistedStmt(int32_t _key, sql::SQLString _query);
	sql::PreparedStatement*		GetStmt(int32_t _key);
};
} // namespace core