module;

// Link: 3rdparty\mysql\lib64\vs14\mysqlcppconn.lib
#include <memory>
#include <string>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_error.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/resultset_metadata.h>
#include <jdbc/cppconn/exception.h>

module makga.lib.database.mysql;

import makga.lib.database.connector;
import makga.lib.logger;

namespace makga::lib::database {
struct MySqlConnector::Impl
{
	sql::Driver* driver = nullptr;
	sql::Connection* conn = nullptr;
};

MySqlConnector::MySqlConnector()
	: impl_(std::make_unique<Impl>())
{
}

MySqlConnector::~MySqlConnector()
{
	Disconnect();
}

bool MySqlConnector::Connect(const DbConfig& config)
{
	try
	{
		impl_->driver = sql::mysql::get_mysql_driver_instance();

		sql::ConnectOptionsMap props;
		props["hostName"] = config.host;
		props["port"] = static_cast<int>(config.port);
		props["userName"] = config.user;
		props["password"] = config.password;
		props["schema"] = config.db_name;
		props["OPT_CHARSET_NAME"] = std::string("utf8mb4");

		impl_->conn = impl_->driver->connect(props);
		return true;
	}
	catch (const sql::SQLException& e)
	{
		MakgaLogger::Error()
			<< "MySqlConnector::Connect failed: " << e.what();
		impl_->conn = nullptr;
		return false;
	}
}

void MySqlConnector::Disconnect()
{
	if (nullptr != impl_->conn)
	{
		try { impl_->conn->close(); }
		catch (...) {}
		delete impl_->conn;
		impl_->conn = nullptr;
	}
}

bool MySqlConnector::IsConnected() const
{
	return (nullptr != impl_->conn) && impl_->conn->isValid();
}

bool MySqlConnector::Execute(const std::string& query, DbRows& out_rows)
{
	try
	{
		std::unique_ptr<sql::Statement>       stmt(impl_->conn->createStatement());
		std::unique_ptr<sql::ResultSet>       res(stmt->executeQuery(query));
		sql::ResultSetMetaData* meta = res->getMetaData();
		const unsigned int                    num_cols = meta->getColumnCount();

		while (res->next())
		{
			DbRow row;
			for (unsigned int i = 1; i <= num_cols; ++i)
			{
				std::string col = meta->getColumnName(i).c_str();
				std::string val = res->isNull(i) ? "" : res->getString(i).c_str();
				row[std::move(col)] = std::move(val);
			}
			out_rows.push_back(std::move(row));
		}
		return true;
	}
	catch (const sql::SQLException& e)
	{
		MakgaLogger::Error()
			<< "MySqlConnector::Execute error: " << e.what();
		return false;
	}
}

bool MySqlConnector::ExecuteNonQuery(const std::string& query)
{
	try
	{
		std::unique_ptr<sql::Statement> stmt(impl_->conn->createStatement());
		stmt->execute(query);
		return true;
	}
	catch (const sql::SQLException& e)
	{
		MakgaLogger::Error()
			<< "MySqlConnector::ExecuteNonQuery error: " << e.what();
		return false;
	}
}
} // namespace makga::lib::database
