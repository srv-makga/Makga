#include "pch.h"
#include "session_community.h"
#include "pool.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::community::RecvPid_##pid, [](SessionCommunity* _procer, std::shared_ptr<Packet> packet) -> bool { return _procer->On##pid(packet); });

bool SessionCommunity::InitDispatcher()
{
	REG_DISPATCHER(Reg);
	return true;
}

SessionCommunity::SessionCommunity()
	: IocpSession(core::ServiceType::IOCP_CLIENT, CONFIG.buffer_size_read)
{
}

SessionCommunity::~SessionCommunity()
{
}

void SessionCommunity::OnConnected()
{
	LOG_INFO << "Connected to SessionCommunity.";
}

void SessionCommunity::OnDisconnected()
{
	LOG_INFO << "Disconnected from SessionCommunity.";
}

std::size_t SessionCommunity::OnRecv(char* buffer, std::size_t _length)
{
	auto packet = POOL.packet.Pop();
	packet->SetBuffer(m_recv_buffer);

	ProcPacket(packet);

	return _length;
}

bool SessionCommunity::ProcPacket(std::shared_ptr<Packet> _packet)
{
	LOG_INFO << "ProcPacket Start. Pid:" << fb::community::EnumNameRecvPid((fb::community::RecvPid)_packet->GetId());

	bool ret = s_dispatcher.Exec((Pid_t)_packet->GetId(), this, _packet);

	LOG_INFO << "ProcPacket End. Pid:" << fb::community::EnumNameRecvPid((fb::community::RecvPid)_packet->GetId()) << "result:" << ret ? "true" : "false";

	return ret;
}

ThreadId_t SessionCommunity::ThreadId() const
{
	return 0;
}

bool SessionCommunity::OnReg(std::shared_ptr<NetPacket> _packet)
{
	LOG_INFO << "Community server connected!";
	return true;
}
