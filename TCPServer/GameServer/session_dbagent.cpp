#include "pch.h"
#include "session_dbagent.h"
#include "pool.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::dbagent::RecvPid_##pid, [](SessionDBAgent* _procer, std::shared_ptr<Packet> packet) -> bool { return _procer->On##pid(packet); });

bool SessionDBAgent::InitDispatcher()
{
	REG_DISPATCHER(Reg);

	return true;
}

SessionDBAgent::SessionDBAgent(std::size_t _buffer_size)
	: IocpSession(Session::Type::IOCPClient, _buffer_size)
{
}

SessionDBAgent::~SessionDBAgent()
{
}

void SessionDBAgent::OnConnected()
{
	LOG_INFO << "Connected to DBAgent.";
}

void SessionDBAgent::OnDisconnected()
{
	LOG_INFO << "Disconnected from DBAgent.";
}

std::size_t SessionDBAgent::OnRecv(char* buffer, std::size_t _length)
{
	auto packet = PacketPool::Instance().Pop();
	packet->SetBuffer(m_recv_buffer);

	ProcPacket(packet);

	return _length;
}

bool SessionDBAgent::ProcPacket(std::shared_ptr<Packet> _packet)
{
	auto pid = (fb::dbagent::RecvPid)_packet->GetId();

	LOG_INFO << "ProcPacket. Pid:" << fb::dbagent::EnumNameRecvPid(pid);

	bool ret = s_dispatcher.Exec(pid, this, _packet);

	LOG_INFO << "ProcPacket. Pid:" << fb::dbagent::EnumNameRecvPid(pid) << ", ret:" << ret ? "true" : "false";

	return ret;
}

bool SessionDBAgent::Send(fb::dbagent::SendPid _pid, fbb& _fbb)
{
	auto buffer = BufferPool::Pop();

	buffer->Write((char*)&_pid, sizeof(_pid));
	buffer->Write((char*)(_fbb.GetBufferPointer()), _fbb.GetSize());

	IocpSession::Send(buffer);

	return true;
}

ThreadId_t SessionDBAgent::ThreadId() const
{
	return ThreadId_t();
}

bool SessionDBAgent::OnReg(std::shared_ptr<NetPacket> _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::dbagent::Recv_Reg);
	return true;
}
