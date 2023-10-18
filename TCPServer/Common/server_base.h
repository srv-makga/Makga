#pragma once

#include "common_type.h"
#include "../Core/net_service.h"
#include <map>

class ConfigBase;
class AcceptorBase;

class ServerBase : public core::network::NetService
{
public:
	ServerBase() = delete;
	ServerBase(const ServerBase&) = delete;
	ServerBase(ServerBase&&) = delete;

	ServerBase& operator=(const ServerBase&) = delete;
	ServerBase& operator=(ServerBase&&) = delete;

	ServerBase(ConfigBase* config);
	virtual ~ServerBase();

	virtual void Initialize();
	virtual void Finallize();

	/*!
	* @details start() 에서 config set 전에 호출되는 함수
	* @detail 주로 데이터를 로드
	*/
	virtual bool StartUp() = 0;

	bool Start();

	/*!
	* @details Start() 에서 config set 후에 호출되는 함수
	* @detail 로드된 데이터로 셋팅
	*/
	virtual bool StartEnd() = 0;

	void Wait();

protected:
	ConfigBase* m_config;
	HANDLE m_service;
};
