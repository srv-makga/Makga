#include "pch.h"
#include "world_session.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::world::RecvPid_##pid, [](WorldSession* _procer, std::shared_ptr<Packet> packet) -> bool { return _procer->On##pid(packet); });

bool WorldSession::InitDispatcher()
{
	REG_DISPATCHER(Reg);
	REG_DISPATCHER(Ping);

	return true;
}

bool WorldSession::OnReg(std::shared_ptr<NetPacket> _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::world::Recv_Reg);

	DLOG_DEBUG << fb::EnumNameeResult(recv_data->result());

	return true;
}

bool WorldSession::OnPing(std::shared_ptr<NetPacket> _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::world::Recv_Ping);

	DLOG_DEBUG << recv_data->time();

	return true;
}