#pragma once

#include "common_header.h"
#include "../Core/object_pool.h"

class SessionBase;

class JobOwner
{
public:
	JobOwner() = default;
	virtual ~JobOwner() = default;

	virtual bool ProcPacket(NetPacket* _packet) = 0;
	virtual ThreadId_t ThreadId() const = 0;
};

//class Job : public core::ObjectPool<Job*>
//{
//public:
//	Job() = default;
//	virtual ~Job() = default;
//
//	virtual void Initialize();
//	virtual void Execute();
//
//	JobOwner* owner;
//	NetPacket* packet;
//};

/*
* @brief: �����͸� �״� Ŭ����
* @detail: �ַ� �����尡 ������ ť���� ���� ���
*/
class JobHandler
{
public:
	JobHandler() {}
	virtual ~JobHandler() {}
	virtual void Push(Job* _data) = 0;
};