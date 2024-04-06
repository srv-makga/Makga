#include "pch.h"
#include "session_dbagent.h"

SessionDBAgent::SessionDBAgent(std::size_t _buffer_length)
	: SessionBase(core::BufferFlag::None, _buffer_length)
{
}

SessionDBAgent::~SessionDBAgent()
{

}

bool SessionDBAgent::RecvPacket(NetPacket* _packet)
{
	Job* job = Job::Pop();
	job->owner = this;
	job->packet = _packet;
	Actor()->JobHandler()->Push(job);

	return true;
}

void SessionDBAgent::OnError(const std::string& _errmsg)
{
	LOG_ERROR << _errmsg;
}

void SessionDBAgent::OnError(std::exception& _exception)
{
	LOG_ERROR << _exception.what();
}

bool SessionDBAgent::ProcPacket(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		return false;
	}

	Pid_t pid = static_cast<Pid_t>(_packet->Id());
	LOG_INFO << "SessionId:" << SessionId() << " ProcPacket Pid:" << fb::dbagent::EnumNameRecvPid(pid);

	if (false == s_dispatcher.Exec(pid, this, _packet))
	{
		LOG_ERROR << "Fail to dispatcher exec. Pid:" << fb::dbagent::EnumNameRecvPid(pid);
		return false;
	}

	return true;
}

ThreadId_t SessionDBAgent::ThreadId() const
{
	return SessionId();
}