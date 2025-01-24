#include "pch.h"
#include "session_world.h"
#include "pool.h"

SessionWorld::SessionWorld(std::size_t _buffer_size)
	: IocpSession(Session::Type::IOCPClient, _buffer_size)
{
}

SessionWorld::~SessionWorld()
{
}

bool SessionWorld::ProcPacket(NetPacket* _packet)
{
	Pid_t pid = static_cast<Pid_t>(_packet->GetId());
	LOG_INFO << "SessionId:" << GetSessionId() << " ProcPacket Pid:" << fb::world::EnumNameRecvPid(pid);

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