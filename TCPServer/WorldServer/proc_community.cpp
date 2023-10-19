#include "pch.h"
#include "session_community.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::world::SendPid_##pid, &SessionCommunity::OnSend_##pid);

bool SessionCommunity::InitDispatcher()
{
	REG_DISPATCHER(Reg);
	REG_DISPATCHER(Ping);

	return true;
}

bool SessionCommunity::OnSend_Reg(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::world::Send_Reg);

	LOG_INFO << "Game Server Connected. type:" << fb::EnumNameeServerType(recv_data->type());

	flatbuffers::FlatBufferBuilder fbb;
	fbb.Finish(fb::world::CreateRecv_Reg(fbb, fb::eResult_Success));
	return Send(fb::world::SendPid_Reg, fbb);
}

bool SessionCommunity::OnSend_Ping(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::world::Send_Ping);

	flatbuffers::FlatBufferBuilder fbb;
	fbb.Finish(fb::world::CreateRecv_Ping(fbb, std::time(nullptr)));
	return Send(fb::world::RecvPid_Ping, fbb);
}