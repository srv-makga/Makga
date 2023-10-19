#include "pch.h"
#include "server_game.h"
#include "config_game.h"
#include "setting_game.h"

#include "../Common/thread_manager.h"
#include "../Common/iocp_Handler.h"

#include "connector_world.h"
#include "connector_community.h"
#include "connector_dbagent.h"

#include "acceptor_user.h"

#include "session_world.h"
#include "session_user.h"

#include "user.h"

ServerGame::ServerGame(ConfigGame* _config)
	: ServerBase(_config)
	, m_job_handler(nullptr)
	, m_net_handler(nullptr)
{
}

ServerGame::~ServerGame()
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

void ServerGame::Initialize()
{
	ServerBase::Initialize();

	ThreadManager* thread_manager = new ThreadManager;
	thread_manager->Initialize();
	thread_manager->InitThread(m_config->thread_count_work);

	m_job_handler = thread_manager;

	m_net_handler = new IOCPHandler();
	m_net_handler->Initialize(m_config->thread_count_network);

	m_connect_list = { eServerType_World/*, ServerType_Community, ServerType_DBAgent */ };
	m_accept_list = { eServerType_User };

	User::InitDispatcher();
	SessionUser::InitDispatcher();
	SessionWorld::InitDispatcher();

	SESSION_WORLD.Initialize();
}

bool ServerGame::StartUp()
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

bool ServerGame::StartEnd()
{
	if (false == InitConnector())
	{
		return false;
	}

	if (false == InitAcceptor())
	{
		return false;
	}

	m_scheduler = std::thread([this]() {
		CREATE_FBB(fbb);
		fbb.Finish(fb::world::CreateSend_Ping(fbb, std::time(nullptr)));

		while (true)
		{
			auto world = this->m_connectors.Find(eServerType_World);
			if (world)
				world->Session()->Send(fb::world::SendPid_Ping, fbb);

			std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
	});

	LOG_INFO << "server start. serverid:" << CONFIG.MyInfo()->ServerId();

	return true;
}

bool ServerGame::InitConnector()
{
	// 해당 서버들에 모두 연결 해야한다
	for (fb::eServerType server_type : m_connect_list)
	{
		const ConfigBase::ServerInfo_t* info = m_config->FindServerInfo(server_type);
		if (nullptr == info)
		{
			LOG_FATAL << "Not found server info. servertype:" << EnumNameeServerType(server_type);
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
		case eServerType_World:
			connector = new ConnectorWorld();
			connector->SetSession(&SESSION_WORLD);
			SESSION_WORLD.SetNetActor(connector);
			break;
		case eServerType_Community:
			/*connector = new ConnectorCommunity();
			connector->SetSession(&SESSION_COMMUNITY);
			SESSION_COMMUNITY.SetNetActor(connector);*/
			break;
		case eServerType_DBAgent:
			//connector = new ConnectorDBAgent();
			//connector->SetSession(&SESSION_DBAGENT);
			//SESSION_DBAGENT.SetNetActor(connector);
			break;
		}

		if (nullptr == connector)
		{
			LOG_FATAL << "connector is nullptr. servertype:" << EnumNameeServerType(server_type);
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

bool ServerGame::InitAcceptor()
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
		case eServerType_User:
		{
			acceptor = new AcceptorUser();
			acceptor->Initialize();
			acceptor->SetServerType(server_type);
			acceptor->InitSession(((ConfigGame*)m_config)->max_session_user_count);
			acceptor->InitHandler(m_net_handler, m_job_handler);
			acceptor->InitSocket(my_info->InternalIP(), port);
			acceptor->InitSetting(((ConfigGame*)m_config)->max_session_user_count);
		}
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
