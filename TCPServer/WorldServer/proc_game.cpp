#include "pch.h"
#include "session_game.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::world::SendPid_##pid, &SessionGame::OnSend_##pid);

bool SessionGame::InitDispatcher()
{
	REG_DISPATCHER(Reg);
	REG_DISPATCHER(Ping);

	return true;
}

bool SessionGame::OnSend_Ping(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::world::Send_Ping);

	flatbuffers::FlatBufferBuilder fbb;
	fbb.Finish(fb::world::CreateRecv_Ping(fbb, std::time(nullptr)));
	return Send(fb::world::RecvPid_Ping, fbb);
}

bool SessionGame::OnSend_Reg(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::world::Send_Reg);

	LOG_INFO << "Game Server Connected. type:" << fb::EnumNameeServerType(recv_data->type());

	flatbuffers::FlatBufferBuilder fbb;
	fbb.Finish(fb::world::CreateRecv_Reg(fbb, fb::eResult_Success));
	return Send(fb::world::SendPid_Reg, fbb);
}