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
* @brief: �����͸� �״� Ŭ����
*/
// @todo JobHandler�� Job(class)�� ThreadPool�� Job(functional)�� ���Ͻ��Ѿ� �Ѵ�.. 
class JobHandler : public core::thread::ThreadPool
{
public:
	using Job_t = std::shared_ptr<Job>;

	JobHandler() = default;
	virtual ~JobHandler() = default;
};