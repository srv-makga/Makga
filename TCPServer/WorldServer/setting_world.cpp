#include "pch.h"
#include "setting_world.h"

SettingWorld::SettingWorld()
{
}

SettingWorld::~SettingWorld()
{
}

void SettingWorld::Initialize(fb::eServerType _server_type)
{
	JsonSaver::Initialize(_server_type);
}

std::string SettingWorld::Serialize()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer writer(buffer);

	m_document.Accept(writer);
	return buffer.GetString();
}

bool SettingWorld::Deserialize()
{
	return true;
}
