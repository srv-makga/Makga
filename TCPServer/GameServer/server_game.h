#pragma once

#include "../core/singleton.hpp"
#include "../Common/Server.h"
#include "../Common/iocp_server.h"
#include "../Common/iocp_client.h"
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

	template<typename T>
	GameServer& AddServer(const std::string& _ip, Port_t _port);

	template<typename T>
	GameServer& AddClient(const std::string& _ip, Port_t _port);

	std::shared_ptr<AppConfig> GetConfig() const;

protected:
	bool StartUp() override;
	bool StartUpEnd() override;

private:
	core::ServiceType m_type;
	std::shared_ptr<AppConfig> m_config;
	std::vector<std::shared_ptr<IocpServer>> m_servers;
	std::vector<std::shared_ptr<IocpClient>> m_clients;
	std::vector<std::shared_ptr<WebClient>> m_web_clients;
	std::shared_ptr<World> m_world;
	std::shared_ptr<DataManager> m_data;
};

#define SERVER	GameServer::Instance()

template<typename T>
inline GameServer& GameServer::AddServer(const std::string& _ip, Port_t _port)
{
	if constexpr (std::is_same_v<T, IocpSession>)
	{
		auto server = std::make_shared<IocpServer>();
		m_servers.push_back(server);
	}
	else if constexpr (std::is_same_v<T, RioSession>)
	{
	}

	return *this;
}

template<typename T>
inline GameServer& GameServer::AddClient(const std::string& _ip, Port_t _port)
{
	if constexpr (std::is_same_v<T, IocpSession>)
	{
		auto client = std::make_shared<IocpClient>();
		m_clients.push_back(client);
	}
	else if constexpr (std::is_same_v<T, RioSession>)
	{
	}

	return *this;
}
