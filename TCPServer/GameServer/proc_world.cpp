#include "pch.h"
#include "session_world.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::world::RecvPid_##pid, [](SessionWorld* _procer, std::shared_ptr<Packet> packet) -> bool { return _procer->On##pid(packet); });

bool SessionWorld::InitDispatcher()
{
	REG_DISPATCHER(Reg);
	REG_DISPATCHER(Ping);

	return true;
}

bool SessionWorld::OnReg(std::shared_ptr<NetPacket> _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::world::Recv_Reg);

	DLOG_DEBUG << fb::EnumNameeResult(recv_data->result());

	return true;
}

bool SessionWorld::OnPing(std::shared_ptr<NetPacket> _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::world::Recv_Ping);

	DLOG_DEBUG << recv_data->time();

	return true;
}