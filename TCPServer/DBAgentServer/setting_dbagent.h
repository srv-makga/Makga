#pragma once

#include "../Common/json_saver.h"
#include "../Core/singleton.hpp"

class SettingDBAgent : public JsonSaver, public core::pattern::Singleton<SettingDBAgent>
{
public:
	SettingDBAgent();
	virtual ~SettingDBAgent();

	void Initialize(fb::eServerType _server_type);

	// memory -> file
	virtual std::string Serialize() override;
	// file -> memory
	virtual bool Deserialize() override;
};

#define SETTING	SettingDBAgent::Instance()