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
	using Job_t = std::shared_ptr<Job>;

	JobHandler() = default;
	virtual ~JobHandler() = default;
	virtual void Push(Job_t _data) = 0;
	virtual Job_t Pop() = 0;
};