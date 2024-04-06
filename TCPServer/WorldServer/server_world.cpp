#include "pch.h"
#include "server_world.h"
#include "config_world.h"
#include "setting_world.h"

#include "thread_manager.h"

#include "../Common/thread_base.h"
#include "../Common/iocp_Handler.h"

#include "connector_dbagent.h"
#include "connector_global.h"

#include "acceptor_community.h"
#include "acceptor_game.h"

#include "session_global.h"
#include "session_dbagent.h"
#include "session_community.h"
#include "session_game.h"

ServerWorld::ServerWorld(ConfigWorld* _config)
	: ServerBase(_config)
	, m_job_handler(nullptr)
	, m_net_handler(nullptr)
{
}

ServerWorld::~ServerWorld()
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

void ServerWorld::Initialize()
{
	ServerBase::Initialize();

	//auto* thread = new ThreadBase();
	//thread->Start();

	auto* thread = new ThreadManager();
	thread->InitThread(2);
	m_job_handler = thread;

	m_net_handler = new IOCPHandler();
	m_net_handler->Initialize(m_config->thread_count_network);

	m_connect_list = { /*eServerType_DBAgent,*/ eServerType_Global};
	m_accept_list = {/*eServerType_Community, eServerType_InstancePool, eServerType_Instance,*/ eServerType_Game };

	SessionGlobal::InitDispatcher();
	SessionDBAgent::InitDispatcher();
	SessionCommunity::InitDispatcher();
	SessionGame::InitDispatcher();

	SESSION_GLOBAL.Initialize();
	SESSION_DBAGENT.Initialize();
}

bool ServerWorld::StartUp()
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

bool ServerWorld::StartEnd()
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

	//m_schedule = std::thread([]() { while (true)
	//	{
	//		DLOG_INFO << "NetPacket size is " << NetPacket::Size(); std::this_thread::sleep_for(std::chrono::minutes(1));
	//	} });

	return true;
}

bool ServerWorld::InitConnector()
{
	for (fb::eServerType server_type : m_connect_list)
	{
		const ConfigBase::ServerInfo_t* info = m_config->FindServerInfo(server_type);
		if (nullptr == info)
		{
			LOG_FATAL << "Not found server info. servertype:" << fb::EnumNameeServerType(server_type);
			return false;
		}

		Port_t port = info->Port(m_config->MyInfo()->ServerType());
		if (0 == port)
		{
			port = info->Port(eServerType_All);
			if (0 == port)
			{
				LOG_FATAL << "Not found connect port";
				return false;
			}
		}

		ConnectorBase* connector = nullptr;

		switch (server_type)
		{
		case eServerType_Global:
			connector = new ConnectorGlobal();
			connector->SetSession(&SESSION_GLOBAL);
			SESSION_GLOBAL.SetNetActor(connector);
			break;
		case eServerType_DBAgent:
			connector = new ConnectorDBAgent();
			connector->SetSession(&SESSION_DBAGENT);
			SESSION_DBAGENT.SetNetActor(connector);
			break;
		}

		if (nullptr == connector)
		{
			LOG_FATAL << "connector is nullptr. servertype:" << fb::EnumNameeServerType(server_type);
			return false;
		}

		connector->InitHandler(m_net_handler, m_job_handler);
		if (false == connector->Connect(info->InternalIP(), port))
		{
			LOG_FATAL << "Fail server connect. host:" << info->InternalIP() << ":" << port;
			return false;
		}

		m_connectors.Add(server_type, connector);
	}

	return true;
}

bool ServerWorld::InitAcceptor()
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
				LOG_FATAL << "Not found port. Servertype:" << fb::EnumNameeServerType(server_type);
				return false;
			}
		}

		AcceptorBase* acceptor = nullptr;

		switch (server_type)
		{
		case eServerType_Community:
			acceptor = new AcceptorCommunity();
			break;
		case eServerType_Game:
			acceptor = new AcceptorGame();
			acceptor->Initialize();
			acceptor->SetServerType(server_type);
			acceptor->InitSession(10);
			acceptor->InitHandler(m_net_handler, m_job_handler);
			acceptor->InitSocket(my_info->InternalIP(), port);
			acceptor->InitSetting(10);
			break;
		}

		if (nullptr == acceptor)
		{
			LOG_FATAL << "acceptor is nullptr. ServerType:" << fb::EnumNameeServerType(server_type);
			return false;
		}
		
		m_acceptors.Add(server_type, acceptor);
	}

	return true;
}
