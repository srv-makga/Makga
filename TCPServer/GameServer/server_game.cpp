#include "pch.h"
#include "server_game.h"
#include "session_manager_user.h"
#include "app_config.h"
#include "pool.h"

GameServer::GameServer(core::ServiceType _type, std::shared_ptr<AppConfig> _config)
	: m_type(_type)
	, m_config(_config)
{
}

GameServer::~GameServer()
{
	m_config = nullptr;
}

bool GameServer::Initialize()
{
	if (nullptr == m_config)
	{
		return false;
	}

	auto lambda = [m_config]() -> bool
		{
		};

	// world
	{
		auto iter = m_config->serverlist_by_type.find(eServerType_World);
		if (m_config->serverlist_by_type.end() == iter)
		{
			LOG_ERROR << "World server not found.";
			return false;
		}

		for (const auto& info : iter->second)
		{
			NetInfo net_info;
			net_info.listen_ip = ::inet_addr(info->InternalIP().c_str());
			net_info.listen_port = info->Port(eServerType_World);
			if (false == m_config->accept_list.insert({ eServerType_World, net_info }).second)
			{
				LOG_ERROR << "World server already exists. server_id:" << info->ServerId();
			}
		}
	}

	
	m_config->serverlist_by_type.find(eServerType_Community);

	// @todo 내가 listen 해야하는 목록을 만든다
	m_config->accept_list.insert(
		{ ServerType_t::eServerType_User, NetInfo{ m_config->listen_ip, m_config->MyInfo()->Port }});

	m_config->serverlist_by_type.find(eServerType_User);
	m_config->serverlist_by_type.find(eServerType_AdminTool);

	// @todo 내가 connect 해야하는 목록을 만든다
	m_config->connector_list;

	return true;
}

void GameServer::Finalize()
{
	m_servers.clear();
	m_clients.clear();
	m_web_clients.clear();
}

std::shared_ptr<AppConfig> GameServer::GetConfig() const
{
	return m_config;
}

bool GameServer::StartUp()
{
	return true;
}

bool GameServer::StartUpEnd()
{
	if (nullptr == m_config)
	{
		return false;
	}

	for (const auto& [type, info] : m_config->accept_list)
	{
		info.listen_ip;
		info.listen_port;
	}

	return true;
}
