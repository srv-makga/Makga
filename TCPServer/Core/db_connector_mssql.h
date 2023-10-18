#pragma once

//#include "db_connector.h"
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

namespace core {
namespace db {
namespace mssql {
//class MssqlConnector : public DBConnector
//{
//public:
//	MssqlConnector();
//	virtual ~MssqlConnector();
//
//	void Initialize();
//
//	bool Disconnect() override;
//	bool IsConnected() const override;
//	bool Reconnect() override;
//
//	Statement* CreateStatement(int32_t _key, const char* _query) override;
//
//protected:
//	bool Connect() override;
//	void ErrorPrint(SQLHSTMT _stmt, SQLHDBC _dbc);
//
//private:
//	SQLHENV m_env;
//	SQLHDBC m_dbc;
//
//	SQLCHAR m_state[512];
//	SQLCHAR m_message[512];
//};
} // namespace maria
} // namespace db
} // namespace core