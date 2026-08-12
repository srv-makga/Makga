module;

// ODBC — Windows SDK 기본 제공, 별도 설치 불필요
// windows.h must precede sql.h: SQLLEN/SQLULEN need BaseTsd.h types, SAL annotations need sal.h
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <memory>
#include <string>
#include <vector>
#include <format>

module makga.lib.database.mssql;

import makga.lib.database.connector;
import makga.lib.conversion_string;
import makga.lib.logger;

namespace makga::lib::database {

struct MssqlConnector::Impl
{
	SQLHENV env       = SQL_NULL_HENV;
	SQLHDBC dbc       = SQL_NULL_HDBC;
	bool    connected = false;
};

MssqlConnector::MssqlConnector()
	: impl_(std::make_unique<Impl>())
{
}

MssqlConnector::~MssqlConnector()
{
	Disconnect();
}

bool MssqlConnector::Connect(const DbConfig& config)
{
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &impl_->env))
	{
		return false;
	}

	SQLSetEnvAttr(impl_->env, SQL_ATTR_ODBC_VERSION,
	              reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0);

	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, impl_->env, &impl_->dbc))
	{
		return false;
	}

	ConversionString conn_str = std::format(
		"DRIVER={{ODBC Driver 17 for SQL Server}};SERVER={},{};DATABASE={};UID={};PWD={};",
		config.host, config.port, config.db_name, config.user, config.password);

	SQLWCHAR out_conn[1024] = { 0, };
	SQLSMALLINT out_len = 0;
	SQLRETURN ret = SQLDriverConnect(
		impl_->dbc, nullptr,
		const_cast<SQLWCHAR*>(conn_str.GetWString().c_str()), SQL_NTS,
		out_conn, static_cast<SQLSMALLINT>(sizeof(out_conn)), &out_len,
		SQL_DRIVER_NOPROMPT);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		MakgaLogger::Error() << "MssqlConnector::Connect failed.";
		return false;
	}

	impl_->connected = true;
	return true;
}

void MssqlConnector::Disconnect()
{
	if (impl_->dbc != SQL_NULL_HDBC)
	{
		SQLDisconnect(impl_->dbc);
		SQLFreeHandle(SQL_HANDLE_DBC, impl_->dbc);
		impl_->dbc = SQL_NULL_HDBC;
	}
	if (impl_->env != SQL_NULL_HENV)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, impl_->env);
		impl_->env = SQL_NULL_HENV;
	}
	impl_->connected = false;
}

bool MssqlConnector::IsConnected() const
{
	return impl_->connected;
}

bool MssqlConnector::Execute(const std::string& query, DbRows& out_rows)
{
	SQLHSTMT stmt = SQL_NULL_HSTMT;
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, impl_->dbc, &stmt))
	{
		return false;
	}

	ConversionString conv_query(query);

	SQLRETURN ret = SQLExecDirect(
		stmt,
		const_cast<SQLWCHAR*>(conv_query.GetWString().c_str()),
		SQL_NTS);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		MakgaLogger::Error() << "MssqlConnector::Execute error.";
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return false;
	}

	SQLSMALLINT num_cols = 0;
	SQLNumResultCols(stmt, &num_cols);
	if (0 == num_cols)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return true;  // INSERT / UPDATE / DELETE
	}

	std::vector<std::string> col_names;
	for (SQLSMALLINT i = 1; i <= num_cols; ++i)
	{
		SQLWCHAR name[256] = {};
		SQLSMALLINT name_len = 0;
		SQLDescribeCol(stmt, i, name, sizeof(name), &name_len,
		               nullptr, nullptr, nullptr, nullptr);
		col_names.emplace_back(ConversionString(std::wstring(name)).GetString());
	}

	while (SQL_SUCCESS == SQLFetch(stmt))
	{
		DbRow row;
		for (SQLSMALLINT i = 1; i <= num_cols; ++i)
		{
			SQLCHAR buf[4096] = {};
			SQLLEN  ind       = 0;
			SQLGetData(stmt, i, SQL_C_CHAR, buf, sizeof(buf), &ind);
			row[col_names[i - 1]] = (SQL_NULL_DATA != ind)
			                      ? reinterpret_cast<char*>(buf) : "";
		}
		out_rows.push_back(std::move(row));
	}

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return true;
}

bool MssqlConnector::ExecuteNonQuery(const std::string& query)
{
	DbRows dummy;
	return Execute(query, dummy);
}
} // namespace makga::lib::database
