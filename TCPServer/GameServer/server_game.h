#pragma once

#include "../core/singleton.hpp"
#include "../Common/Server.h"
#include "../Common/iocp_server.h"
#include "../Common/iocp_client.h"
#include "../Common/web_client.h"
#include "world.h"
#include "data_manager.h"

class GameServer : public Server, public core::pattern::Singleton<GameServer>
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

protected:
	bool StartUp() override;
	bool StartUpEnd() override;

private:
	void ReleaseServer();
	void ReleaseClient();

private:
	core::ServiceType m_type;
	std::shared_ptr<AppConfig> m_config;
	std::shared_ptr<core::network::IocpCore> m_iocp_core;

	std::map<eServerType, std::shared_ptr<Server_t>> m_servers;
	std::map<eServerType, std::shared_ptr<Client_t>> m_clients;
	std::map<eServerType, std::shared_ptr<WebClient>> m_web_clients;

	std::shared_ptr<World> m_world;
	std::shared_ptr<DataManager> m_data;
};

#define SERVER	GameServer::Instance()