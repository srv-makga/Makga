#include "pch.h"
#include "session_community.h"
#include "pool.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::community::RecvPid_##pid, [](CommunityServer* _procer, std::shared_ptr<Packet> packet) -> bool { return _procer->On##pid(packet); });

bool CommunityServer::InitDispatcher()
{
	REG_DISPATCHER(Reg);
	return true;
}

CommunityServer::CommunityServer()
	: IocpSession(Session::Type::IOCPClient, CONFIG.buffer_size_read)
{
}

CommunityServer::~CommunityServer()
{
}

void CommunityServer::OnConnected()
{
	LOG_INFO << "Connected to CommunityServer.";
}

void CommunityServer::OnDisconnected()
{
	LOG_INFO << "Disconnected from CommunityServer.";
}

std::size_t CommunityServer::OnRecv(char* buffer, std::size_t _length)
{
	auto packet = PacketPool::Instance().Pop();
	packet->SetBuffer(m_recv_buffer);

	ProcPacket(packet);

	return _length;
}

bool CommunityServer::ProcPacket(std::shared_ptr<Packet> _packet)
{
	LOG_INFO << "ProcPacket Start. Pid:" << fb::community::EnumNameRecvPid((fb::community::RecvPid)_packet->GetId());

	bool ret = s_dispatcher.Exec((Pid_t)_packet->GetId(), this, _packet);

	LOG_INFO << "ProcPacket End. Pid:" << fb::community::EnumNameRecvPid((fb::community::RecvPid)_packet->GetId()) << "result:" << ret ? "true" : "false";

	return ret;
}

ThreadId_t CommunityServer::ThreadId() const
{
	return 0;
}

bool CommunityServer::OnReg(std::shared_ptr<NetPacket> _packet)
{
	LOG_INFO << "Community server connected!";
	return true;
}
