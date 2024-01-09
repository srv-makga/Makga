#pragma once

#include "common_const.h"
#include "../Core/RapidJson.h"
#include "../Core/core_struct.h"
#include "../Core/logger_glog.h"

#include <map>

class ItemProperty;

class ServerInfo
{
public:
	ServerInfo() = default;
	~ServerInfo() = default;

	void Initialize()
	{
		server_id = 0;
		server_type = fb::eServerType_None;
		internal_ip.clear();
		external_ip.clear();
		ports.clear();
	}

	bool InitJsonValue(const rapidjson::Value& _value)
	{
		rapidjson::Document document;
		document.CopyFrom(_value, document.GetAllocator());
		
		auto obj = document.GetObj();

		server_id = std::atoi(GetJson(document, "server_id", ""));

		server_type = STR_TO_ENUM(eServerType, GetJson(document, "type", ""));
		internal_ip = GetJson(document, "internal_ip", "127.0.0.1");
		external_ip = GetJson(document, "external_ip", "127.0.0.1");

		if (false == document.HasMember("port") || false == document["port"].IsObject())
		{
			LOG_ERROR << "Not found port.";
			return false;
		}

		try
		{
			const rapidjson::Value& port_obj = obj["port"];
			for (auto iter = port_obj.MemberBegin(); port_obj.MemberEnd() != iter; ++iter)
			{
				ServerType_t server_type = STR_TO_ENUM(eServerType, iter->name.GetString());
				
				Port_t port = iter->value.GetInt();
				if (1000 > port)
				{
					LOG_ERROR << "Port is invalid. port:" << port;
				}

				ports.insert({server_type, port});
			}

			if (true == ports.empty())
			{
				return false;
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << e.what();
			return false;
		}

		return true;
	}

	ServerId_t ServerId() const { return server_id; }
	ServerType_t ServerType() const { return server_type; }
	const std::string& InternalIP() const { return internal_ip; }
	const std::string& ExternalIP() const { return external_ip; }
	Port_t Port(ServerType_t _server_type) const
	{
		auto iter = ports.find(_server_type);
		if (ports.end() == iter)
		{
			return Port_t();
		}

		return iter->second;
	}

private:
	ServerId_t server_id;
	ServerType_t server_type;
	std::string internal_ip;
	std::string external_ip;
	std::map<ServerType_t, Port_t> ports;
};

struct DBInfo
{
	std::string host;
	uint16_t port = 0;
	std::string id;
	std::string pw;
	std::string db;
	std::string encoding;
};

struct Position
{
	Coord_t x;
	Coord_t y;
	Coord_t z;
};

struct PositionXY
{
	Coord_t x;
	Coord_t y;
};

struct MonsterCreateOption
{
	TableIdx_t monster_idx;
	Vector_t position;
	Angle_t angle; // 생성시 방향
};

struct ItemCreateOption
{

};

using UmapItemList = std::unordered_map<ItemIdx_t, StackCount_t>;
using UmapItemPropertyList = std::unordered_map<const ItemProperty*, StackCount_t>;