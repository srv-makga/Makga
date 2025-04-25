#pragma once

#include "../Common/config.h"
#include "../Common/iocp_server.h"
#include "../Common/iocp_client.h"

using Server_t = IocpServer;
using Client_t = IocpClient;
using Session_t = core::network::IocpSession;

struct AppConfig : public ServerConfig<Session_t>, public std::enable_shared_from_this<AppConfig>
{
	int max_actor_monster = 10000;
	int max_actor_npc = 1000;
	int max_actor_object = 1000;
};