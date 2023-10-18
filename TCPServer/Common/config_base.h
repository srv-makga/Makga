#pragma once

#include "common_header.h"
#include "../Core/singleton.hpp"
#include "../Core/RapidJson.h"
#include "../Core/json_object.hpp"
#include <map>

class ConfigBase
{
public:
	using ServerInfo_t = ::ServerInfo;
	using DBInfo_t = ::DBInfo;

	bool Parsing(fb::eServerType _type, int _argc, char** _argv);
	bool LoadServerInfo(const std::string& _file_name, fb::eServerType _server_type);
	bool LoadServerList(const std::string& _file_name);

	bool AddServerInfo(ServerInfo_t* _server_info);
	const ServerInfo_t* FindServerInfo(ServerId_t _server_id);
	const ServerInfo_t* FindServerInfo(fb::eServerType _server_type);
	const ServerInfo_t* MyInfo() const;

	bool AddDBInfo(fb::eDBType _type, DBInfo_t* _db_info);
	bool AddRedisInfo(fb::eRedisType _type, DBInfo_t* _db_info);

	/////////////////////////////////////////////////////////////////////////////////

	ServerId_t server_id = 0;
	fb::eServerType server_type = fb::eServerType_None;

	// default
	String8 app_name = "";
	String8 locale = "";

	// log
	String8 log_path = "log";
	std::size_t log_console_level = 4;
	std::size_t log_max_file_size = 1024; // MB

	std::size_t pool_session_init_count = 3000;
	std::size_t pool_packet_init_count = 1 << 12;

	// thread
	std::size_t thread_count_network = 1;
	std::size_t thread_count_work = 1;
	std::size_t thread_count_database = 1;

	// buffer
	std::size_t buffer_size_write = 4096;
	std::size_t buffer_size_read = 4096;

	std::map<ServerId_t, ServerInfo_t*> serverlist;
	std::map<ServerType_t, ServerInfo_t*> serverlist_by_type; // 타입별 서버 (여러개 쓰려면 프록시 서버를 만든다)

	std::map<fb::eDBType, DBInfo_t*> db_list;
	std::map<fb::eRedisType, DBInfo_t*> redis_list;


	const ServerInfo_t* my_server = nullptr;
};

