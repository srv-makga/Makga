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

void SessionWorld::OnConnected()
{
	LOG_INFO << "Connected to World.";
}

void SessionWorld::OnDisconnected()
{
	LOG_INFO << "Disconnected from World.";
}

std::size_t SessionWorld::OnRecv(char* buffer, std::size_t _length)
{
	auto packet = PacketPool::Instance().Pop();
	packet->SetBuffer(m_recv_buffer);

	ProcPacket(packet);

	return _length;
}

bool SessionWorld::ProcPacket(std::shared_ptr<NetPacket> _packet)
{
	Pid_t pid = static_cast<Pid_t>(_packet->GetId());

	LOG_INFO << "ProcPacket. Pid:" << fb::world::EnumNameRecvPid(pid);

	bool ret = s_dispatcher.Exec(pid, this, _packet);
	
	LOG_INFO << "ProcPacket. Pid:" << fb::world::EnumNameRecvPid(pid) << " ret:" << ret;

	return ret;
}

ThreadId_t SessionWorld::ThreadId() const
{
	return 0;
}