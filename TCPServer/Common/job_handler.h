#pragma once

#include "common_header.h"
#include "job.h"
#include "../core/thread_pool.hpp"

class JobOwner
{
public:
	JobOwner() = default;
	virtual ~JobOwner() = default;

	virtual bool ProcPacket(std::shared_ptr<NetPacket> _packet) = 0;
	virtual ThreadId_t ThreadId() const = 0;
};

class JobHandler
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