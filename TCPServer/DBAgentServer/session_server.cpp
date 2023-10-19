#include "pch.h"
#include "session_server.h"

SessionServer::SessionServer(std::size_t _buffer_size)
	: SessionBase(core::BufferFlag::None, _buffer_size)
{
}

SessionServer::~SessionServer()
{
}

bool SessionServer::RecvPacket(NetPacket* _packet)
{
	// @todo 워크스레드 작업
	return ProcPacket(_packet);
}

bool SessionServer::ProcPacket(NetPacket* _packet)
{
	if (nullptr == _packet)
	{
		return false;
	}

	auto pid = static_cast<fb::dbagent::SendPid>(_packet->Id());
	LOG_INFO << "ProcPacket. Pid:" << fb::dbagent::EnumNameSendPid(pid);

	if (false == s_dispatcher.Exec(pid, this, _packet))
	{
		LOG_ERROR << "Fail to dispatcher exec. Pid:" << fb::dbagent::EnumNameSendPid(pid);
	}

	NetPacket::Push(_packet);

	return true;
}

ThreadId_t SessionServer::ThreadId() const
{
	return 0;
}
