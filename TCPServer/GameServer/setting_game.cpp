#include "pch.h"
#include "setting_game.h"

SettingGame::SettingGame()
{
}

SettingGame::~SettingGame()
{
}

void SettingGame::Initialize(fb::eServerType _server_type)
{
	JsonSaver::Initialize(_server_type);
}

std::string SettingGame::Serialize()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer writer(buffer);

	m_document.Accept(writer);
	return buffer.GetString();
}

bool SettingGame::Deserialize()
{
	return true;
}
