#pragma once

#include "dbagent_header.h"
#include "../Core/db_connector.h"
#include "../Core/db_statement.h"
#include "../Core/dispatcher.h"

// @brief 패킷 처리 함수 (aka DBProc)
class DatabaseClient
{
public:
	using Pid_t = fb::dbagent::SendPid;
	using Function_t = bool (DatabaseClient::*)(SessionBase*, NetPacket*);
	using STMTS = std::map<Pid_t, core::db::Statement*>;
public:
	static bool InitDispatcher();

private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	DatabaseClient();
	virtual ~DatabaseClient();

	void Initialize();
	bool Connecter(const DBInfo* _info);
	bool Disconnect();

	bool ProcPacket(SessionBase* _session, NetPacket* _packet);
	bool SendPing();

protected:
	DECLARE_RECV_DBA(Reg);
	DECLARE_RECV_DBA(LoginAuth);

private:
	core::db::DBConnector* m_connector;

	STMTS m_stmts;
};