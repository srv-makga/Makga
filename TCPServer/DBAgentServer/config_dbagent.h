#pragma once

#include "../Common/config_base.h"
#include "../Core/singleton.hpp"

struct ConfigDBAgent : public ConfigBase, public core::pattern::Singleton<ConfigDBAgent>
{
	std::size_t session_user_buffer_size = 8192;
};

#define CONFIG	ConfigDBAgent::Instance()