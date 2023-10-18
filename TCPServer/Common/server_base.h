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
	* @details start() ���� config set ���� ȣ��Ǵ� �Լ�
	* @detail �ַ� �����͸� �ε�
	*/
	virtual bool StartUp() = 0;

	bool Start();

	/*!
	* @details Start() ���� config set �Ŀ� ȣ��Ǵ� �Լ�
	* @detail �ε�� �����ͷ� ����
	*/
	virtual bool StartEnd() = 0;

	void Wait();

protected:
	ConfigBase* m_config;
	HANDLE m_service;
};
