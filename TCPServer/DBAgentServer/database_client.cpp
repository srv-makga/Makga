#include "pch.h"
#include "database_client.h"
#include "config_dbagent.h"
#include "../Core/db_connector_mysql.h"

DatabaseClient::DatabaseClient()
{
}

DatabaseClient::~DatabaseClient()
{
	delete m_connector;
}

void DatabaseClient::Initialize()
{
	if (nullptr != m_connector && true == m_connector->IsConnected())
	{
		m_connector->Disconnect();
		delete m_connector;
	}

	m_connector = new core::db::mysql::MysqlConnector();
}

bool DatabaseClient::Connecter(const DBInfo* _info)
{
	if (nullptr == m_connector)
	{
		LOG_ERROR << "m_connector is nullptr.";
		return false;
	}

	if (true == m_connector->IsConnected())
	{
		m_connector->Disconnect();
	}

	if (false == m_connector->Connect(_info->host, _info->port, _info->id, _info->pw, _info->db, _info->encoding))
	{
		LOG_ERROR << "Fail to RDB Connect."
			<< " host:" << _info->host
			<< " port:" << _info->port
			<< " db:" << _info->db;

		return false;
	}

	return true;
}

bool DatabaseClient::Disconnect()
{
    return false;
}

bool DatabaseClient::ProcPacket(SessionBase* _session, NetPacket* _packet)
{
	Pid_t pid = static_cast<Pid_t>(_packet->Id());

	LOG_INFO << "ProcPacket Pid:" << fb::dbagent::EnumNameSendPid(pid);

	if (false == s_dispatcher.Exec(pid, this, _session, _packet))
	{
		LOG_ERROR << "Not defined packet id. " << fb::dbagent::EnumNameSendPid(pid);
		return false;
	}

	return true;
}

bool DatabaseClient::SendPing()
{
    return false;
}