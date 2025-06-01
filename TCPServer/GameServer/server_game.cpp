#include "pch.h"
#include "server_game.h"
#include "app_config.h"
#include "pool.h"
#include "app_session.h"
//#include "session_world.h"
//#include "session_community.h"
//#include "session_dbagent.h"
//#include "session_admintool.h"
//#include "session_user.h"

// 남은 작업
// NetHandler
// WorkHandler 만들고 연결하기
// 로직단에서 GameServer에 대한 접근 가능 하도록

GameServer::GameServer(core::ServiceType _type, std::shared_ptr<AppConfig> _config)
	: m_type(_type) // @todo 해당 타입 값으로 내부 서버를 지정가능해야함
	, m_config(_config)
	, m_iocp_core(nullptr)
{
}

GameServer::~GameServer()
{
	m_config->connector_list.clear();
	//m_config->accept_list.clear();
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
	//m_config->accept_list.clear();

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

std::shared_ptr<AppClient> GameServer::GetDBAgentServer() const
{
	auto iter = m_clients.find(eServerType_DBAgent);
	if (m_clients.end() == iter)
	{
		LOG_ERROR << "DBAgent server not found.";
		return nullptr;
	}

	return iter->second;
}

std::shared_ptr<AppConfig> GameServer::GetConfig() const
{
	return m_config;
}

bool GameServer::StartUp()
{
	// 처음 session 할당을 설정할 때, 미리 정해진 갯수 만큼 만들어 둔다.
	// 보통 최대 접속 수는 정해져 있으니, 설정할 때 최대 갯수로 만들어 주니까,
	// 추가 할당의 케이스를 고려하지 않아도 되지 않을까?

	// @todo IocpServer, IocpClient의 StartUp, StartUpEnd가 가상함수다.
	// @todo ServerType 이름을 NetActorType으로 변경할 필요가 있다.
	auto connect_lambda = [this]<typename T>(eServerType _type) -> bool
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
				std::shared_ptr<AppClient> client = std::make_shared<AppClient>();
				if (nullptr == client)
				{
					LOG_ERROR << "Client create failed.";
					return false;
				}

				client->Initialize();

				core::network::IPEndPoint end_point(info->IP(_type), info->Port(_type));

				if (false == client->Setup(m_iocp_core, end_point, [this]() -> std::shared_ptr<core::network::IocpSession> { return std::make_shared<T>(m_config->buffer_size_read); }, [](std::shared_ptr<core::network::IocpSession> _session) { _session.reset(); }))
				{
					LOG_ERROR << "Already setup. type:" << type_name << " ip:" << end_point.Ip() << ", port : " << end_point.Port();
					continue;
				}

				if (false == m_clients.insert({ _type, client }).second)
				{
					LOG_ERROR << "Already exists. type:" << type_name << " ip:" << end_point.Ip() << ", port : " << end_point.Port();
					continue;
				}
			}

			return true;
		};

	if (false == connect_lambda.operator()<SessionWorld>(eServerType_World))
	{
		return false;
	}

	if (false == connect_lambda.operator()<SessionCommunity>(eServerType_Community))
	{
		return false;
	}

	if (false == connect_lambda.operator()<SessionDBAgent>(eServerType_DBAgent))
	{
		return false;
	}

	auto acceptor_lambda = [this]<typename T>(eServerType _type, AcceptorInfo&& _info, 
		std::function<std::shared_ptr<Session_t>()> _alloc, 
		std::function<void(std::shared_ptr<Session_t>)> _dealloc) -> bool
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
				std::shared_ptr<Server_t> server = std::make_shared<Server_t>();
				if (nullptr == server)
				{
					LOG_ERROR << "Server create failed.";
					return false;
				}

				server->Initialize();
				server->SetMaxSessionCount(_info.max_connection);
				server->SetThreadCount(_info.thread_count);

				core::network::IPEndPoint end_point(_info.listen_ip, _info.listen_port);

				// core를 여러개로 만들면 각자의 iocp handler로 처리된다
				if (false == server->Setup(m_iocp_core, end_point, _alloc, _dealloc))
				{
					LOG_ERROR << "Already setup server. type:" << EnumNameeServerType(_type) << " ep:" << end_point.GetString();
					return false;
				}

				if (false == m_servers.insert({ _type, server }).second)
				{
					LOG_ERROR << "Already exists. type:" << EnumNameeServerType(_type) << " ep:" << end_point.GetString();
					return false;
				}
			}

			return true;
		};
	
	/* @todo 유저 세션 매니저가 필요하다 */
	if (false == acceptor_lambda.operator()<SessionUser>(eServerType_User, AcceptorInfo{
			//.listen_ip = m_config->MyInfo()->IP(eServerType_User),
			//.listen_port = m_config->MyInfo()->Port(eServerType_User),
			.max_connection = m_config->session_init_count, 
			.thread_count = m_config->thread_count_network
		}
		, [this]() { return std::make_shared<SessionUser>(m_config->buffer_size_read); }
		, [this](std::shared_ptr<Session_t> _session) { _session.reset(); })
	)
	{
		return false;
	}

	if (false == acceptor_lambda.operator()<AppSession>(eServerType_AdminTool, AcceptorInfo{
			//.listen_ip = m_config->MyInfo()->IP(eServerType_AdminTool),
			//.listen_port = m_config->MyInfo()->Port(eServerType_AdminTool),
			.max_connection = 1,
			.thread_count = 1
		}
		, [this]() { return std::make_shared<SessionAdmintool>(m_config->buffer_size_read); }
		, [this](std::shared_ptr<Session_t> _session) { _session.reset(); })
	)
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

	for (auto& [type, client] : m_clients)
	{
		if (false == client->Start())
		{
			LOG_ERROR << "Fail start client. type:" << EnumNameeServerType(type) << " ep:" << client->GetEndPoint().GetString();
		}
	}

	for (auto& [type, server] : m_servers)
	{
		if (false == server->Start())
		{
			LOG_ERROR << "Fail start client. type:" << EnumNameeServerType(type) << " ep:" << server->GetEndPoint().GetString();
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

	return true;
}

void GameServer::ReleaseServer()
{
	if (false == m_servers.empty())
	{
		for (auto& iter : m_servers)
		{
			iter.second->Stop();
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
			iter.second->Stop();
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
