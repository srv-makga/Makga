#pragma once

#include "../Common/json_saver.h"
#include "../Core/singleton.hpp"

class SettingGlobal : public JsonSaver, public core::pattern::Singleton<SettingGlobal>
{
public:
	SettingGlobal();
	virtual ~SettingGlobal();

	void Initialize(fb::eServerType _server_type);

	// memory -> file
	virtual std::string Serialize() override;
	// file -> memory
	virtual bool Deserialize() override;
};

#define SETTING	SettingGlobal::Instance()