#pragma once

#include "common_header.h"
#include "job_handler.h"
#include "../Core/net_core.h"

/*
* @brief ��Ʈ��ũ �����Ͱ� ó���� Ŭ������ �������̽� Ŭ����
* @detail SelectEvent, IocpEvent���� ���� Ŭ������ �����
* @todo ���Ŀ� IOData�� Job���� �޴� ������� ������ ����?
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