#include "pch.h"
#include "session_world.h"

SessionWorld::SessionWorld(std::size_t _buffer_size)
	: SessionBase(core::BufferFlag::None, _buffer_size)
{
	
}

SessionWorld::~SessionWorld()
{

}

bool SessionWorld::RecvPacket(NetPacket* _packet)
{
	Job* job = Job::Pop();
	job->Initialize();
	job->owner = this;
	job->packet = _packet;

	Actor()->JobHandler()->Push(job);
	return true;
}

bool SessionWorld::ProcPacket(NetPacket* _packet)
{
	Pid_t pid = static_cast<Pid_t>(_packet->Id());
	LOG_INFO << "SessionId:" << SessionId() << " ProcPacket Pid:" << fb::world::EnumNameRecvPid(pid);

	if (false == s_dispatcher.Exec(pid, this, _packet))
	{
		LOG_ERROR << "Fail to dispatcher exec. Pid:" << fb::world::EnumNameRecvPid(pid);
		return false;
	}

	return true;
}

ThreadId_t SessionWorld::ThreadId() const
{
	return 0;
}