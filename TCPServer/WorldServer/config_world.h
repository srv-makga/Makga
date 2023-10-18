#pragma once

#include "../Common/config_base.h"
#include "../Core/singleton.hpp"

struct ConfigWorld : public ConfigBase, public core::pattern::Singleton<ConfigWorld>
{
	std::size_t session_user_buffer_size = 8192;
};

#define CONFIG	ConfigWorld::Instance()