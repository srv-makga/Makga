#pragma once

#include "../Common/json_saver.h"
#include "../Core/singleton.hpp"

class SettingGame : public JsonSaver, public core::pattern::Singleton<SettingGame>
{
public:
	SettingGame();
	virtual ~SettingGame();

	void Initialize(fb::eServerType _server_type);

	// memory -> file
	virtual std::string Serialize() override;
	// file -> memory
	virtual bool Deserialize() override;
};

#define SETTING	SettingGame::Instance()