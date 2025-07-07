#pragma once

#include "common_header.h"
#include "job_handler.h"
#include "../Core/net_core.h"

/*
* @brief 네트워크 데이터가 처리될 클래스의 인터페이스 클래스
* @detail SelectEvent, IocpEvent등의 세부 클래스가 상속함
* @todo 추후에 IOData를 Job으로 받는 방식으로 변경할 예정?
*/
class NetHandler
{
public:
	NetHandler() = default;
	virtual ~NetHandler() = default;

public:
	virtual bool Initialize(std::shared_ptr<core::network::NetCore> _net_core) = 0;
	virtual void Finalize() = 0;
	virtual bool CreateThread(std::size_t _thread_count = 1) = 0;
	virtual bool Start() = 0;
	virtual void Stop() = 0;
};