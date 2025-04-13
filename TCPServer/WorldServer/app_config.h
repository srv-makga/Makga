#pragma once

#include "../Common/config.h"
#include "../Core/singleton.hpp"

struct AppConfig : public Config, public core::pattern::Singleton<AppConfig>
{
	std::size_t session_user_buffer_size = 8192;
};

#define CONFIG	AppConfig::Instance()