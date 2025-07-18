#pragma once

#include "common_header.h"
#include "../core/thread_pool.h"

class Job;

class JobOwner
{
public:
	JobOwner() = default;
	virtual ~JobOwner() = default;

	virtual bool ProcPacket(std::shared_ptr<NetPacket> _packet) = 0;
	virtual ThreadId_t ThreadId() const = 0;
};

/*
* @brief: 데이터를 쌓는 클래스
*/
// @todo JobHandler의 Job(class)과 ThreadPool의 Job(functional)을 통일시켜야 한다.. 
class JobHandler : public core::thread::ThreadPool
{
public:
	JobHandler() = default;
	virtual ~JobHandler() = default;

	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;
	virtual bool CreateThread(std::size_t _thread_count = 1) = 0;
	virtual void Push(std::shared_ptr<Job> _job) = 0;

protected:
	virtual std::shared_ptr<Job> Pop() = 0;
};