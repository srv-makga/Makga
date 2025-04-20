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
	m_config->connector_list.clear();
	m_config->accept_list.clear();
	m_config = nullptr;

	m_servers.clear();
	m_clients.clear();
	m_web_clients.clear();

	m_world = nullptr;
	m_data = nullptr;
}

bool GameServer::Initialize()
{
	if (nullptr == m_config)
	{
		return false;
	}

	m_config->connector_list.clear();
	m_config->accept_list.clear();

	m_servers.clear();
	m_clients.clear();
	m_web_clients.clear();

	m_world = std::make_shared<World>();
	if (false == m_world->Initialize())
	{
		LOG_ERROR << "world initialize failed.";
		return false;
	}

	m_data = std::make_shared<DataManager>();
	if (false == m_data->Initialize("datatable"))
	{
		LOG_ERROR << "data initialize failed.";
		return false;
	}

	return true;
}

void GameServer::Finalize()
{
	m_servers.clear();
	m_clients.clear();
	m_web_clients.clear();

	m_world = nullptr;
	m_data = nullptr;
}

std::shared_ptr<AppConfig> GameServer::GetConfig() const
{
	return m_config;
}

bool GameServer::StartUp()
{
	auto connect_lambda = [this](eServerType _type) -> bool
		{
			const char* type_name = EnumNameeServerType(_type);

			auto iter = m_config->serverlist_by_type.find(_type);
			if (m_config->serverlist_by_type.end() == iter)
			{
				LOG_ERROR << type_name << " not found.";
				return false;
			}

			for (const auto& info : iter->second)
			{
				NetInfo net_info;
				net_info.listen_ip = ::inet_addr(info->InternalIP().c_str()); // @todo ip가 내부일지 외부일지 알 수 없다
				net_info.listen_port = info->Port(_type);
				if (false == m_config->connector_list.insert({ _type, net_info }).second)
				{
					LOG_ERROR << type_name << " already exists. server_id:" << info->ServerId();
				}
			}

			return true;
		};

	if (false == connect_lambda(eServerType_World))
	{
		return false;
	}

	if (false == connect_lambda(eServerType_Community))
	{
		return false;
	}

	if (false == connect_lambda(eServerType_DBAgent))
	{
		return false;
	}

	auto acceptor_lambda = [this](eServerType _type) -> bool
		{
			const char* type_name = EnumNameeServerType(_type);

			auto iter = m_config->serverlist_by_type.find(_type);
			if (m_config->serverlist_by_type.end() == iter)
			{
				LOG_ERROR << type_name << " not found.";
				return false;
			}

			for (const auto& info : iter->second)
			{
				NetInfo net_info;
				net_info.listen_ip = ::inet_addr(info->ExternalIP().c_str()); // @todo ip가 내부일지 외부일지 알 수 없다
				net_info.listen_port = info->Port(_type);
				if (false == m_config->accept_list.insert({ _type, net_info }).second)
				{
					LOG_ERROR << type_name << " already exists. server_id:" << info->ServerId();
				}
			}

			return true;
		};

	// 내가 listen 해야하는 목록을 만든다
	if (false == acceptor_lambda(eServerType_User))
	{
		return false;
	}

	if (false == acceptor_lambda(eServerType_AdminTool))
	{
		return false;
	}

	return true;
}

bool GameServer::StartUpEnd()
{
	if (nullptr == m_config)
	{
		return false;
	}

	for (const auto& connect_info : m_config->connector_list)
	{
		const NetInfo& info = connect_info.second;
		core::network::IPEndPoint end_point(info.remote_ip, info.remote_port);

		std::shared_ptr<IocpClient> client = std::make_shared<IocpClient>();
		if (nullptr == client)
		{
			LOG_ERROR << "IocpClient create failed.";
			return false;
		}

		client->Initialize();
		client->SetEndPoint(end_point);
		client->Start();

		m_clients.push_back(client);
	}

	//for (const auto& connect_info : m_config->web_connector_list)
	//{
	//	const NetInfo& info = connect_info.second;
	//	core::network::IPEndPoint end_point(info.remote_ip, info.remote_port);

	//	std::shared_ptr<WebClient> client = std::make_shared<WebClient>();
	//	if (nullptr == client)
	//	{
	//		LOG_ERROR << "IocpClient create failed.";
	//		return false;
	//	}

	//	client->Initialize();
	//	client->SetEndPoint(end_point);
	//	client->Start();

	//	m_web_clients.push_back(client);
	//}

	for (const auto& accept_info : m_config->accept_list)
	{
		const NetInfo& info = accept_info.second;
		core::network::IPEndPoint end_point(info.listen_ip, info.listen_port);

		std::shared_ptr<IocpServer> server = std::make_shared<IocpServer>();
		if (nullptr == server)
		{
			LOG_ERROR << "IocpServer create failed.";
			return false;
		}

		server->Initialize();
		server->SetEndPoint(end_point);
		server->Start();

		m_servers.push_back(server);
	}

	return true;
}
