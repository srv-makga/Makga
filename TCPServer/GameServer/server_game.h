#pragma once

#include "../core/singleton.hpp"
#include "../Common/Server.h"
#include "../Common/iocp_server.h"
#include "../Common/iocp_client.h"

class GameServer : public Server, public core::pattern::Singleton<GameServer>
{
public:
	GameServer(core::ServiceType _type);
	GameServer(const GameServer& _other) = delete;
	GameServer(GameServer&& _other) = delete;
	GameServer& operator=(const GameServer& _other) = delete;
	GameServer& operator=(GameServer&& _other) = delete;
	virtual ~GameServer();

	bool Initialize() override;
	void Finalize() override;

	template<typename T>
	GameServer& AddServer(const std::string& _ip, Port_t _port);

protected:
	bool StartUp() override;
	bool StartUpEnd() override;

private:
	std::vector<std::shared_ptr<IocpServer>> m_servers;
	std::vector<std::shared_ptr<IocpClient>> m_clients;
	std::vector<std::shared_ptr<WebClient>> m_web_clients;
	std::shared_ptr<World> m_world;
	std::shared_ptr<Data> m_data;
};

#define SERVER	GameServer::Instance()

template<typename T>
inline GameServer& GameServer::AddServer(const std::string& _ip, Port_t _port)
{
	return *this;
}
