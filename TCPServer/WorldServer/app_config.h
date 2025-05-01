#pragma once

#include "../Common/config.h"
#include "../Core/singleton.hpp"

struct AppConfig : public Config, public std::enable_shared_from_this<AppConfig>
{
	std::size_t session_user_buffer_size = 8192;
};

#define CONFIG	AppConfig::Instance()