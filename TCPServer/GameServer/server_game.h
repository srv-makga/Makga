#pragma once

#include "../core/singleton.hpp"
#include "../Common/Server.h"
#include "../Common/web_client.h"
#include "app_server.h"
#include "app_client.h"
#include "app_session.h"
#include "world.h"
#include "data_manager.h"

class GameServer : public Server, public std::enable_shared_from_this<GameServer>
{
public:
	GameServer(core::ServiceType _type, std::shared_ptr<AppConfig> _config);
	GameServer(const GameServer& _other) = delete;
	GameServer(GameServer&& _other) = delete;
	GameServer& operator=(const GameServer& _other) = delete;
	GameServer& operator=(GameServer&& _other) = delete;
	virtual ~GameServer();

	bool Initialize() override;
	void Finalize() override;

	std::shared_ptr<AppConfig> GetConfig() const;

protected: // Server
	bool StartUp() override;
	bool StartUpEnd() override;

private:
	bool StartConnect(fb::eServerType _type);
	bool StartConnect(ConnectorInfo&& _info);
	bool StartAccept(AcceptorInfo&& _info);
	void ReleaseServer();
	void ReleaseClient();

	std::shared_ptr<AppSession> CreateSession(fb::eServerType _type);
	
	const core::network::IPEndPoint& GetEndPoint(eServerType _type) const;

	std::shared_ptr<AppClient> FindClient(eServerType _type);

public:
	inline static std::shared_ptr<GameServer> s_server;
	static std::shared_ptr<GameServer> Instance() { return s_server; }

	static AppClient* GetDBAgentServer();

private:
	core::ServiceType m_type;
	std::shared_ptr<AppConfig> m_config;
	std::shared_ptr<core::network::IocpCore> m_iocp_core;

	std::shared_ptr<NetHandler> m_net_handler;
	std::shared_ptr<JobHandler> m_user_job_handler;
	std::shared_ptr<JobHandler> m_server_job_handler;

	std::map<eServerType, std::shared_ptr<AppServer>> m_servers;
	std::map<eServerType, std::shared_ptr<AppClient>> m_clients;
	std::map<eServerType, std::shared_ptr<WebClient>> m_web_clients;

	std::shared_ptr<World> m_world;
	std::shared_ptr<DataManager> m_data;
};