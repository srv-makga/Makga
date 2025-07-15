#pragma once

#include "common_header.h"
#include "../Core/singleton.hpp"
#include "../Core/RapidJson.h"
#include "../Core/json_object.hpp"
#include <map>

class NetHandler;
class JobHandler;

struct NetInfo
{
	union
	{
		u_long listen_ip;
		u_long remote_ip;
	};

	union
	{
		Port_t listen_port;
		Port_t remote_port;
	};
};

struct DatabaseInfo : public NetInfo
{
	std::string database_name;
};

struct ConnectorInfo : public NetInfo
{
	fb::eServerType server_type = fb::eServerType_None;
	std::shared_ptr<NetHandler> net_handler = nullptr;
	std::shared_ptr<JobHandler> job_handler = nullptr;
};

struct AcceptorInfo : public NetInfo
{
	fb::eServerType server_type = fb::eServerType_None;
	std::size_t max_connection = 0;
	std::size_t max_session_buffer = 0;

	std::shared_ptr<NetHandler> net_handler = nullptr;
	std::shared_ptr<JobHandler> job_handler = nullptr;
};

class Config
{
public:
	using ServerInfo_t = ::ServerInfo;
	using DBInfo_t = ::DBInfo;
	using ConnectorList = std::map<eServerType, NetInfo>;

	bool Parsing(fb::eServerType _type, ServerId_t _serverid);
	bool LoadServerInfo(const std::string& _file_name, fb::eServerType _server_type);
	bool LoadServerList(const std::string& _file_name);

	bool AddServerInfo(ServerInfo_t* _server_info);
	const ServerInfo_t* FindServerInfo(ServerId_t _server_id);
	const ServerInfo_t* FindServerInfo(fb::eServerType _server_type);
	const ServerInfo_t* MyInfo() const;

	bool AddDBInfo(fb::eDatabaseType _type, DBInfo_t* _db_info);
	bool AddRedisInfo(fb::eRedisType _type, DBInfo_t* _db_info);

	/////////////////////////////////////////////////////////////////////////////////

	ServerId_t server_id = 0;
	const ServerInfo_t* my_info = nullptr;
	fb::eServerType server_type = fb::eServerType_None;

	// default
	String8 app_name = "";
	String8 locale = "";

	// log
	String8 log_path = "log";
	std::size_t log_console_level = 4;
	std::size_t log_max_file_size = 1024; // MB

	// session
	std::size_t session_init_count = 3000;
	std::size_t pool_packet_init_count = 1 << 12;

	// thread
	std::size_t thread_count_network = 1;
	std::size_t thread_count_work = 1;
	std::size_t thread_count_database = 1;

	// buffer
	std::size_t buffer_size_write = 1 << 13;
	std::size_t buffer_size_read = 1 << 13;

	std::map<ServerId_t, ServerInfo_t*> serverlist;
	std::map<ServerType_t, std::vector<ServerInfo_t*>> serverlist_by_type; // 타입별 서버 (여러개 쓰려면 프록시 서버를 만든다)

	std::map<fb::eDatabaseType, DBInfo_t*> db_list;
	std::map<fb::eRedisType, DBInfo_t*> redis_list;

	ConnectorList connector_list;
	ConnectorList web_connector_list;
};

class ServerConfig : public Config
{
	using DatabaseList = std::map<eDatabaseType, DatabaseInfo>;

public:
	DatabaseList	database_list;
};