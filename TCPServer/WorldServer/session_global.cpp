#include "pch.h"
#include "session_global.h"

SessionGlobal::SessionGlobal(std::size_t _buffer_length)
	: SessionBase(core::BufferFlag::None, _buffer_length)
{
}

SessionGlobal::~SessionGlobal()
{

}

bool SessionGlobal::RecvPacket(NetPacket* _packet)
{
	Job* job = Job::Pop();
	job->owner = this;
	job->packet = _packet;
	Actor()->JobHandler()->Push(job);

	return true;
}

bool SessionGlobal::ProcPacket(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		return false;
	}

	Pid_t pid = static_cast<Pid_t>(_packet->Id());
	LOG_INFO << "SessionId:" << SessionId() << " ProcPacket Pid:" << fb::global::EnumNameRecvPid(pid);

	if (false == s_dispatcher.Exec(pid, this, _packet))
	{
		LOG_ERROR << "Fail to dispatcher exec. Pid:" << fb::global::EnumNameRecvPid(pid);
		return false;
	}

	return true;
}

ThreadId_t SessionGlobal::ThreadId() const
{
	return SessionId();
}