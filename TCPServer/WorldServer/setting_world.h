#pragma once

#include "../Common/json_saver.h"
#include "../Core/singleton.hpp"

class SettingWorld : public JsonSaver, public core::pattern::Singleton<SettingWorld>
{
public:
	SettingWorld();
	virtual ~SettingWorld();

	void Initialize(fb::eServerType _server_type);

	// memory -> file
	virtual std::string Serialize() override;
	// file -> memory
	virtual bool Deserialize() override;
};

#define SETTING	SettingWorld::Instance()