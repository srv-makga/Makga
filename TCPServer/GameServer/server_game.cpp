#include "pch.h"
#include "server_game.h"
#include "session_manager_user.h"
#include "app_config.h"
#include "pool.h"

// 남은 작업
// NetHandler
// WorkHandler 만들고 연결하기
// 로직단에서 GameServer에 대한 접근 가능 하도록

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
	// @todo ServerType 이름을 NetActorType으로 변경할 필요가 있다.

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

	// 각 세션 타입별로 hanler를 따로 빼야하는데
	// 각 세션을 상속받아 쓸 것인가 아니면 => 상속별 클래스가 많아짐
	// 핸들로 함수를 람다로 전달할 것인가 선택 필요
	// => 람다를 할 경우 각 Server와 Client에 저장이 필요한데
	// 실제로 핸들러는 세션에서 처리됨
	// 각 Server와 Client가 세션을 생성할 때 핸들러 람다를 지정할 수 있어야함
	// 근데 alloc과 dealloc만 있으니 내부에서 실제 생성은 하지 않음
	// 그럼 애초에 풀에서 실제로 할당할 때 만들어서 풀링을 해야하는데...

	/*
	* 세션으로 유저같은 상위 객체 찾기 필요.
	* 상위 객체가 어느 스레드에서 처리되는지 찾기
	* 상위 객체의 패킷 핸들러로 처리 보내기

	* 핸들러 처리가 필요한 타입
	* - World
	* - Community
	* - DBAgent
	* - User
	* - AdminTool
	*/

	// 패킷 처리가 필요한 객체들은 별도의 부모 클래스가 하나 필요하다...

	// NetActor 별로 차이점
	// 1. 버퍼 크기 => 생성자
	// 2. 워크 스레드 =>
	// 3. 핸들러 => 생성자

	auto temp_alloc = [this]()-> std::shared_ptr<Session_t>
		{
			auto session = std::make_shared<Session_t>(core::ServiceType::IOCP_CLIENT, m_config->buffer_size_read);
			session->proc_packet_func = [](std::shared_ptr<IocpSession> _session, Packet* _packet) -> bool
				{
					// 패킷을 처리할 수 있는 객체는 모두 Messager를 상속 받아야함
					// ProcPacket 함수 안에서 워크 스레드로 넘기는 작업
					// 핸들러로 넘기는 작업이 모두 필요함

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

		// core를 여러개로 만들면 각자의 iocp handler로 처리된다
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
