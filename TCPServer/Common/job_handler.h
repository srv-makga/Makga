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
* @brief: 데이터를 쌓는 클래스
* @detail: 주로 스레드가 데이터 큐잉을 위해 사용
*/
class JobHandler
{
public:
	JobHandler() {}
	virtual ~JobHandler() {}
	virtual void Push(std::shared_ptr<Job> _data) = 0;
};