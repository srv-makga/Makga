#include "pch.h"
#include "server_dbagent.h"
#include "config_dbagent.h"
#include "setting_dbagent.h"

#include "database_handler.h"
#include "../Common/iocp_Handler.h"
#include "thread_manager.h"

#include "acceptor_server.h"

#include "session_server.h"

#include "database_client.h"

ServerDBAgent::ServerDBAgent(ConfigDBAgent* _config)
	: ServerBase(_config)
	, m_job_handler(nullptr)
	, m_net_handler(nullptr)
{
}

ServerDBAgent::~ServerDBAgent()
{
	for (fb::eServerType type : m_accept_list)
	{
		AcceptorBase* acceptor = m_acceptors.Find(type);
		if (nullptr == acceptor)
		{
			continue;
		}

		m_acceptors.Remove(type);

		acceptor->Finalize();
		delete acceptor;
	}

	for (fb::eServerType type : m_connect_list)
	{
		ConnectorBase* connector = m_connectors.Find(type);
		if (nullptr == connector)
		{
			continue;
		}

		m_connectors.Remove(type);

		connector->Finalize();
		delete connector;
	}
}

void ServerDBAgent::Initialize()
{
	ServerBase::Initialize();

	DatabaseHandler* job_handler = new DatabaseHandler();
	job_handler->InitThread(m_config->thread_count_work);
	m_job_handler = job_handler;

	m_net_handler = new IOCPHandler();
	m_net_handler->Initialize(m_config->thread_count_network);

	m_connect_list = { };
	m_accept_list = { eServerType_All };

	SessionServer::InitDispatcher();
	DatabaseClient::InitDispatcher();
}

bool ServerDBAgent::StartUp()
{
	// #################################################################################### //
	// Server Setting
	// #################################################################################### //

	if (nullptr == m_config->MyInfo())
	{
		LOG_ERROR << "Not found server info. server_id:" << m_config->server_id;
		return false;
	}

	return true;
}

bool ServerDBAgent::StartEnd()
{
	if (false == InitConnector())
	{
		LOG_ERROR << "Fail to InitConnector.";
		return false;
	}

	if (false == InitAcceptor())
	{
		LOG_ERROR << "Fail to InitAcceptor.";
		return false;
	}

	LOG_INFO << "server start. serverid:" << CONFIG.MyInfo()->ServerId();

	return true;
}

bool ServerDBAgent::InitConnector()
{
	return true;
}

bool ServerDBAgent::InitAcceptor()
{
	const auto* my_info = m_config->MyInfo();

	for (fb::eServerType server_type : m_accept_list)
	{
		Port_t port = my_info->Port(server_type);
		if (0 == port)
		{
			port = my_info->Port(fb::eServerType_All);
			if (0 == port)
			{
				LOG_FATAL << "Not found port. Servertype:" << EnumNameeServerType(server_type);
				return false;
			}
		}

		AcceptorBase* acceptor = nullptr;

		switch (server_type)
		{
		case eServerType_All:
			acceptor = new AcceptorServer();
			acceptor->Initialize();
			acceptor->SetServerType(server_type);
			acceptor->InitSession(/*@todo 임시*/10);
			acceptor->InitHandler(m_net_handler, m_job_handler);
			acceptor->InitSocket(my_info->InternalIP(), port);
			acceptor->InitSetting(/*@todo 임시*/10);
			break;
		}

		if (nullptr == acceptor)
		{
			LOG_FATAL << "acceptor is nullptr. ServerType:" << EnumNameeServerType(server_type);
			return false;
		}
		
		m_acceptors.Add(server_type, acceptor);
	}

	return true;
}
