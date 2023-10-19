#include "pch.h"
#include "session_server.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::dbagent::SendPid_##pid, &SessionServer::On##pid);

core::Dispatcher<SessionServer::Pid_t, SessionServer::Function_t> SessionServer::s_dispatcher;

bool SessionServer::InitDispatcher()
{
	REG_DISPATCHER(Reg);
	
	return true;
}

bool SessionServer::OnReg(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::dbagent::Send_Reg);

	LOG_INFO << "Server Connected. type:" << EnumNameeServerType(recv_data->type());

	flatbuffers::FlatBufferBuilder fbb;
	fbb.Finish(fb::dbagent::CreateRecv_Reg(fbb, fb::eResult_Success));
	return Send(fb::dbagent::SendPid_Reg, fbb);
}