#pragma once

#include "common_header.h"
#include "../core/thread_pool.h"

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
*/
// @todo JobHandler의 Job(class)과 ThreadPool의 Job(functional)을 통일시켜야 한다.. 
class JobHandler : public core::thread::ThreadPool
{
public:
	using Job_t = std::shared_ptr<Job>;

	JobHandler() = default;
	virtual ~JobHandler() = default;
};