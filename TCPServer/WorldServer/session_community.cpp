#include "pch.h"
#include "session_community.h"

SessionCommunity::SessionCommunity(std::size_t _buffer_size)
	: SessionBase(core::BufferFlag::None, _buffer_size)
{
}

SessionCommunity::~SessionCommunity()
{
}

bool SessionCommunity::RecvPacket(NetPacket* _packet)
{
	Job* job = Job::Pop();
	job->owner = this;
	job->packet = _packet;
	Actor()->JobHandler()->Push(job);

	return true;
}

bool SessionCommunity::ProcPacket(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		return false;
	}

	Pid_t pid = static_cast<Pid_t>(_packet->Id());
	LOG_INFO << "SessionId:" << SessionId() << " ProcPacket Pid:" << fb::world::EnumNameSendPid(pid);

	if (false == s_dispatcher.Exec(pid, this, _packet))
	{
		LOG_ERROR << "Fail to dispatcher exec. Pid:" << fb::world::EnumNameSendPid(pid);
		return false;
	}

	return true;
}

void SessionCommunity::OnError(const char* _msg)
{
	LOG_ERROR << _msg;
}

void SessionCommunity::OnError(std::exception& _exception)
{
	LOG_ERROR << _exception.what();
}

ThreadId_t SessionCommunity::ThreadId() const
{
	return SessionId();
}
