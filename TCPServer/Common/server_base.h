#pragma once

#include "common_type.h"
#include "../Core/net_service.h"

class ConfigBase;

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

public:
	void Initialize();
	void Finalize();

	// @brief ���� ����
	bool Start();

	// @brief ���� �̺�Ʈ ���
	void Wait();

	// @brief ���� ����
	void Stop();

public:
	// @detail �ַ� �����͸� �ε�
	virtual bool StartUp() = 0;
	// @detail �ε�� �����ͷ� ����
	virtual bool StartEnd() = 0;

protected:
	ConfigBase* m_config;

	std::mutex m_exit_mutex;
	std::condition_variable m_exit_cv;
};
