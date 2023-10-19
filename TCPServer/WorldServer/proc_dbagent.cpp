#include "pch.h"
#include "session_dbagent.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::dbagent::RecvPid_##pid, &SessionDBAgent::OnRecv_##pid);

bool SessionDBAgent::InitDispatcher()
{
	REG_DISPATCHER(Reg);
	REG_DISPATCHER(Ping);

	return true;
}

bool SessionDBAgent::OnRecv_Reg(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::dbagent::Recv_Reg);

	LOG_INFO << "DBAgent Server Connected.";

	return true;
}

bool SessionDBAgent::OnRecv_Ping(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::dbagent::Recv_Ping);

	DLOG_INFO << recv_data->time();

	return true;
}