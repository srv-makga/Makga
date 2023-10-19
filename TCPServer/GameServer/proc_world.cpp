#include "pch.h"
#include "session_world.h"

bool SessionWorld::InitDispatcher()
{
	s_dispatcher.Add(fb::world::RecvPid_Reg, &SessionWorld::OnRecv_Reg);
	s_dispatcher.Add(fb::world::RecvPid_Ping, &SessionWorld::OnRecv_Ping);

	return true;
}

bool SessionWorld::OnRecv_Reg(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::world::Recv_Reg);

	DLOG_DEBUG << fb::EnumNameeResult(recv_data->result());

	return true;
}

bool SessionWorld::OnRecv_Ping(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::world::Recv_Ping);

	DLOG_DEBUG << recv_data->time();

	return true;
}