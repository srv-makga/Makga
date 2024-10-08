#pragma once

#include "../Common/config_base.h"
#include "../Core/singleton.hpp"

struct ConfigGame : public ConfigBase, public core::pattern::Singleton<ConfigGame>
{
	int max_session_user_count = 3000;
	int max_actor_monster = 10000;
	int max_actor_npc = 1000;
	int max_actor_object = 1000;

	std::size_t buffer_size_user = 1 << 16;
};

#define CONFIG		ConfigGame::Instance()
#define SERVERLIST	CONFIG.serverlist