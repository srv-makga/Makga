#include "pch.h"
#include "server_game.h"
#include "session_manager_user.h"
#include "app_config.h"
#include "pool.h"

// ���� �۾�
// NetHandler
// WorkHandler ����� �����ϱ�
// �����ܿ��� GameServer�� ���� ���� ���� �ϵ���

GameServer::GameServer(core::ServiceType _type, std::shared_ptr<AppConfig> _config)
	: m_type(_type)
	, m_config(_config)
	, m_iocp_core(nullptr)
{
}

GameServer::~GameServer()
{
	m_config->connector_list.clear();
	m_config->accept_list.clear();
	m_config = nullptr;

	m_iocp_core = nullptr;

	ReleaseServer();
	ReleaseClient();

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

	m_iocp_core = std::make_shared<core::network::IocpCore>();
	if (false == m_iocp_core->Initialize())
	{
		return false;
	}

	ReleaseServer();
	ReleaseClient();
	
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
	m_iocp_core = nullptr;

	ReleaseServer();
	ReleaseClient();

	m_world = nullptr;
	m_data = nullptr;
}

std::shared_ptr<AppConfig> GameServer::GetConfig() const
{
	return m_config;
}

bool GameServer::StartUp()
{
	// @todo ServerType �̸��� NetActorType���� ������ �ʿ䰡 �ִ�.

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
				net_info.listen_ip = ::inet_addr(info->InternalIP().c_str()); // @todo ip�� �������� �ܺ����� �� �� ����
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

	auto acceptor_lambda = [this](eServerType _type, AcceptorInfo<Session_t>&& _info) -> bool
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
				_info.listen_ip = ::inet_addr(info->ExternalIP().c_str());
				_info.listen_port = info->Port(_type);

				if (false == m_config->accept_list.insert({ _type, _info }).second)
				{
					LOG_ERROR << type_name << " already exists. server_id:" << info->ServerId();
				}
			}

			return true;
		};

	if (false == acceptor_lambda(eServerType_User, AcceptorInfo<Session_t>{
		.max_connection = m_config->session_init_count, 
		.thread_count = m_config->thread_count_network
	}))
	{
		return false;
	}

	if (false == acceptor_lambda(eServerType_AdminTool, AcceptorInfo<Session_t>{
		.max_connection = 1,
		.thread_count = 1
	}))
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

	// �� ���� Ÿ�Ժ��� hanler�� ���� �����ϴµ�
	// �� ������ ��ӹ޾� �� ���ΰ� �ƴϸ� => ��Ӻ� Ŭ������ ������
	// �ڵ�� �Լ��� ���ٷ� ������ ���ΰ� ���� �ʿ�
	// => ���ٸ� �� ��� �� Server�� Client�� ������ �ʿ��ѵ�
	// ������ �ڵ鷯�� ���ǿ��� ó����
	// �� Server�� Client�� ������ ������ �� �ڵ鷯 ���ٸ� ������ �� �־����
	// �ٵ� alloc�� dealloc�� ������ ���ο��� ���� ������ ���� ����
	// �׷� ���ʿ� Ǯ���� ������ �Ҵ��� �� ���� Ǯ���� �ؾ��ϴµ�...

	/*
	* �������� �������� ���� ��ü ã�� �ʿ�.
	* ���� ��ü�� ��� �����忡�� ó���Ǵ��� ã��
	* ���� ��ü�� ��Ŷ �ڵ鷯�� ó�� ������

	* �ڵ鷯 ó���� �ʿ��� Ÿ��
	* - World
	* - Community
	* - DBAgent
	* - User
	* - AdminTool
	*/

	// ��Ŷ ó���� �ʿ��� ��ü���� ������ �θ� Ŭ������ �ϳ� �ʿ��ϴ�...

	// NetActor ���� ������
	// 1. ���� ũ�� => ������
	// 2. ��ũ ������ =>
	// 3. �ڵ鷯 => ������

	auto temp_alloc = [this]()-> std::shared_ptr<Session_t>
		{
			auto session = std::make_shared<Session_t>(core::ServiceType::IOCP_CLIENT, m_config->buffer_size_read);
			session->proc_packet_func = [](std::shared_ptr<IocpSession> _session, Packet* _packet) -> bool
				{
					// ��Ŷ�� ó���� �� �ִ� ��ü�� ��� Messager�� ��� �޾ƾ���
					// ProcPacket �Լ� �ȿ��� ��ũ ������� �ѱ�� �۾�
					// �ڵ鷯�� �ѱ�� �۾��� ��� �ʿ���

					Messager* messager = MessagerMgr->FIndBySession(_session);
					if (nullptr == messager)
					{
						messager = _session;
					}

					return _session->ProcPacket(_packet);
				};
		};

	for (const auto& [type, info] : m_config->connector_list)
	{
		core::network::IPEndPoint end_point(info.remote_ip, info.remote_port);

		std::shared_ptr<Client_t> client = std::make_shared<Client_t>();
		if (nullptr == client)
		{
			LOG_ERROR << "Client create failed.";
			return false;
		}

		client->Initialize();

		if (false == client->Setup(m_iocp_core, end_point, ))
		{
			LOG_ERROR << "Already setup. type:" << EnumNameeServerType(type) << " ip:" << info.listen_ip << ", port : " << info.listen_port;
			continue;
		}

		if (false == m_clients.insert({ type, client }).second)
		{
			LOG_ERROR << "Already exists. type:" << EnumNameeServerType(type) << " ip:" << info.remote_ip << ", port : " << info.remote_port;
			continue;
		}

		if (false == client->Start())
		{
			LOG_ERROR << "Fail start client. type:" << EnumNameeServerType(type) << " ip:" << info.remote_ip << ", port : " << info.remote_port;
		}
	}

	for (const auto& [type, info] : m_config->web_connector_list)
	{
		std::shared_ptr<WebClient> client = std::make_shared<WebClient>(info.remote_ip, info.remote_port);
		if (nullptr == client)
		{
			LOG_ERROR << "WebClient create failed.";
			return false;
		}

		client->Initialize();

		if (false == m_web_clients.insert({ type, client }).second)
		{
			LOG_ERROR << EnumNameeServerType(type) << " already exists. ip:" << info.remote_ip << ", port:" << info.remote_port;
			return false;
		}
	}

	for (const auto& [type, info] : m_config->accept_list)
	{
		core::network::IPEndPoint end_point(info.listen_ip, info.listen_port);

		std::shared_ptr<Server_t> server = std::make_shared<Server_t>();
		if (nullptr == server)
		{
			LOG_ERROR << "Server create failed.";
			return false;
		}

		server->Initialize();
		server->SetMaxSessionCount(info.session_count);
		server->SetThreadCount(info.thread_count);

		// core�� �������� ����� ������ iocp handler�� ó���ȴ�
		if (false == server->Setup(m_iocp_core, end_point, info.alloc_session, info.dealloc_session))
		{
			LOG_ERROR << "Already setup server. type:" << EnumNameeServerType(type) << " ip:" << info.listen_ip << ", port : " << info.listen_port;
			return false;
		}

		if (false == m_servers.insert({ type, server }).second)
		{
			LOG_ERROR << "Already exists. type:" << EnumNameeServerType(type) << " ip:" << info.listen_ip << ", port : " << info.listen_port;
			return false;
		}

		if (false == server->Start())
		{
			LOG_ERROR << "Fail start server. type:" << EnumNameeServerType(type) << " ip:" << info.listen_ip << ", port : " << info.listen_port;
			return false;
		}
	}

	return true;
}

void GameServer::ReleaseServer()
{
	if (false == m_servers.empty())
	{
		for (auto& iter : m_servers)
		{
			iter.second = nullptr;
		}

		m_servers.clear();
	}
}

void GameServer::ReleaseClient()
{
	if (false == m_clients.empty())
	{
		for (auto& iter : m_clients)
		{
			iter.second = nullptr;
		}

		m_clients.clear();
	}

	if (false == m_web_clients.empty())
	{
		for (auto& iter : m_web_clients)
		{
			iter.second = nullptr;
		}

		m_web_clients.clear();
	}
}
