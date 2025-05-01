#pragma once

#include "../Common/config.h"
#include "../Core/singleton.hpp"

struct AppConfig : public Config, public std::enable_shared_from_this<AppConfig>
{
	int max_actor_monster = 10000;
	int max_actor_npc = 1000;
	int max_actor_object = 1000;

	std::size_t session_user_buffer_size = 1 << 13;
};

#define SERVERLIST	CONFIG.serverlist