#pragma once

#include "common_header.h"
#include "../Core/object_pool.h"

class SessionBase;
class Job;

class JobOwner
{
public:
	JobOwner() = default;
	virtual ~JobOwner() = default;

	virtual bool ProcPacket(NetPacket* _packet) = 0;
	virtual ThreadId_t ThreadId() const = 0;
};

/*
* @brief: �����͸� �״� Ŭ����
* @detail: �ַ� �����尡 ������ ť���� ���� ���
*/
class JobHandler
{
public:
	JobHandler() {}
	virtual ~JobHandler() {}
	virtual void Push(std::shared_ptr<Job> _data) = 0;
};