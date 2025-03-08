#include "pch.h"
#include "server_game.h"
#include "session_manager_user.h"
#include "config_game.h"
#include "pool.h"

GameServer::GameServer(core::ServiceType _type)
	: ProxyServer(_type)
{
}

GameServer::~GameServer()
{
}

bool GameServer::Initialize()
{
	if (false == m_acceptor_user.Initialize())
	{
		return false;
	}

	if (false == m_acceptor_admintool.Initialize())
	{
		return false;
	}

	return true;
}

void GameServer::Finalize()
{
	m_acceptor_user.Finalize();
	m_acceptor_admintool.Finalize();
}

bool GameServer::StartUp()
{
	return true;
}

bool GameServer::StartEnd()
{
	auto my_server = CONFIG.my_server;
	if (nullptr == my_server)
	{
		return false;
	}

	Connector connector;

	m_session_world = connector.Connect<SessionWorld>(core::network::IPEndPoint(my_server->InternalIP(), my_server->Port(ServerType_t::eServerType_World)));
	if (nullptr == m_session_world)
	{
		LOG_ERROR << "Failed to connect world server.";
		return false;
	}

	m_session_community = connector.Connect<SessionCommunity>(core::network::IPEndPoint(my_server->InternalIP(), my_server->Port(ServerType_t::eServerType_Community)));
	if (nullptr == m_session_community)
	{
		LOG_ERROR << "Failed to connect community server.";
		return false;
	}

	////�α� ����
	//auto session_log = connector.Connect<SessionLog>(core::network::IPEndPoint(my_server->InternalIP(), my_server->Port(ServerType_t::eServerType_Log)));
	//if (nullptr == session_log)
	//{
	//	LOG_ERROR << "Failed to connect log server.";
	//	return false;
	//}

	////ä�� ����
	//auto session_chat = connector.Connect<SessionChat>(core::network::IPEndPoint(my_server->InternalIP(), my_server->Port(ServerType_t::eServerType_Chat)));
	//if (nullptr == session_chat)
	//{
	//	LOG_ERROR << "Failed to connect chat server.";
	//	return false;
	//}

	////���� ����(+�� ����)
	//auto session_billing = connector.Connect<SessionBilling>(core::network::IPEndPoint(my_server->InternalIP(), my_server->Port(ServerType_t::eServerType_Billing)));
	//if (nullptr == session_billing)
	//{
	//	LOG_ERROR << "Failed to connect billing server.";
	//	return false;
	//}

	m_session_dbagent = connector.Connect<SessionDBAgent>(core::network::IPEndPoint(my_server->InternalIP(), my_server->Port(ServerType_t::eServerType_DBAgent)));
	if (nullptr == m_session_dbagent)
	{
		LOG_ERROR << "Failed to connect dbagent server.";
		return false;
	}

	//����


	// @todo �� iocp �������� ������ ������ �� �ִ� ��� �߰�
	// �Ǵ� Ư�� �����忡 ���� iocp�� ó���� �� �ֵ��� �ϴ� ��ĵ� ���
	m_acceptor_user.Setup(core::network::IPEndPoint(my_server->ExternalIP(), my_server->Port(ServerType_t::eServerType_User)));
	m_acceptor_admintool.Setup(core::network::IPEndPoint(my_server->InternalIP(), my_server->Port(ServerType_t::eServerType_AdminTool)));

	return true;
}
