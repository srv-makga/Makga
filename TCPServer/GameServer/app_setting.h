#pragma once

#include "../Common/json_saver.h"
#include "../Core/singleton.hpp"

class AppSetting : public JsonSaver, public core::pattern::Singleton<AppSetting>
{
public:
	AppSetting();
	virtual ~AppSetting();

	void Initialize(fb::eServerType _server_type);

	// memory -> file
	virtual std::string Serialize() override;
	// file -> memory
	virtual bool Deserialize() override;
};

#define SETTING	AppSetting::Instance()