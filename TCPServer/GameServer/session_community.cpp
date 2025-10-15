#include "pch.h"
#include "session_community.h"
#include "connector_community.h"

core::Dispatcher<CommunityServer::Pid_t, CommunityServer::Function_t> CommunityServer::s_dispatcher;

bool CommunityServer::InitDispatcher()
{
	s_dispatcher.Add(fb::community::RecvPid_Reg, &CommunityServer::OnReg);
	return true;
}

CommunityServer::CommunityServer()
	: SessionBase(core::BufferFlag::None, CONFIG.buffer_size_read)
{
}

CommunityServer::~CommunityServer()
{
}

bool CommunityServer::RecvPacket(NetPacket* _packet)
{
	return ProcPacket(_packet);
}

bool CommunityServer::ProcPacket(NetPacket* _packet)
{
	bool ret = s_dispatcher.Exec((Pid_t)_packet->Id(), this, _packet);
	NetPacket::Push(_packet);
	return ret;
}

ThreadId_t CommunityServer::ThreadId() const
{
	return 0;
}

bool CommunityServer::OnReg(NetPacket* _packet)
{
	LOG_INFO << "World server connected!";
	return true;
}
