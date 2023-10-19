#include "pch.h"
#include "setting_dbagent.h"

SettingDBAgent::SettingDBAgent()
{
}

SettingDBAgent::~SettingDBAgent()
{
}

void SettingDBAgent::Initialize(fb::eServerType _server_type)
{
	JsonSaver::Initialize(_server_type);
}

std::string SettingDBAgent::Serialize()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer writer(buffer);

	m_document.Accept(writer);
	return buffer.GetString();
}

bool SettingDBAgent::Deserialize()
{
	return true;
}
