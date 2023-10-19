#include "pch.h"
#include "session_global.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::global::RecvPid_##pid, &SessionGlobal::OnRecv_##pid);

bool SessionGlobal::InitDispatcher()
{
	REG_DISPATCHER(Reg);
	REG_DISPATCHER(Ping);

	return true;
}

bool SessionGlobal::OnRecv_Reg(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::global::Recv_Reg);

	LOG_INFO << "Global Server Connected.";

	return true;
}

bool SessionGlobal::OnRecv_Ping(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::global::Recv_Ping);

	DLOG_INFO << recv_data->time();

	return true;
}