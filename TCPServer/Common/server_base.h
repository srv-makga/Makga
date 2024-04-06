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

	// @brief 서버 시작
	bool Start();

	// @brief 종료 이벤트 대기
	void Wait();

	// @brief 서버 정지
	void Stop();

public:
	// @detail 주로 데이터를 로드
	virtual bool StartUp() = 0;
	// @detail 로드된 데이터로 셋팅
	virtual bool StartEnd() = 0;

protected:
	ConfigBase* m_config;

	std::mutex m_exit_mutex;
	std::condition_variable m_exit_cv;
};
