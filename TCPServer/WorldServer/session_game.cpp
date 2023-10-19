#include "pch.h"
#include "session_game.h"

SessionGame::SessionGame(std::size_t _buffer_size)
	: SessionBase(core::BufferFlag::None, _buffer_size)
{
}

SessionGame::~SessionGame()
{
}

bool SessionGame::RecvPacket(NetPacket* _packet)
{
	Job* job = Job::Pop();
	job->owner = this;
	job->packet = _packet;
	Actor()->JobHandler()->Push(job);

	return true;
}

bool SessionGame::ProcPacket(NetPacket* _packet)
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

ThreadId_t SessionGame::ThreadId() const
{
	return SessionId();
}
