#include "pch.h"
#include "session_dbagent.h"
#include "connector_dbagent.h"

core::Dispatcher<SessionDBAgent::Pid_t, SessionDBAgent::Function_t> SessionDBAgent::s_dispatcher;

bool SessionDBAgent::InitDispatcher()
{
	s_dispatcher.Add(fb::dbagent::RecvPid_Reg, &SessionDBAgent::OnRecv_Reg);

	return true;
}

SessionDBAgent::SessionDBAgent(std::size_t _buffer_size)
	: SessionBase(core::BufferFlag::None, _buffer_size)
{
}

SessionDBAgent::~SessionDBAgent()
{
}

bool SessionDBAgent::RecvPacket(NetPacket* _packet)
{
	Job* job = Job::Pop();
	job->Initialize();
	job->owner = this;
	job->packet = _packet;

	Actor()->JobHandler()->Push(job);
	return true;
}

bool SessionDBAgent::ProcPacket(NetPacket* _packet)
{
	auto pid = (fb::dbagent::RecvPid)_packet->Id();

	LOG_INFO << "ProcPacket. Pid:" << fb::dbagent::EnumNameRecvPid(pid);

	bool ret = s_dispatcher.Exec(pid, this, _packet);
	LOG_ERROR_IF(false == ret) << "Fail to dispatcher exec. Pid:" << fb::dbagent::EnumNameRecvPid(pid);

	return ret;
}

ThreadId_t SessionDBAgent::ThreadId() const
{
	return ThreadId_t();
}

bool SessionDBAgent::OnRecv_Reg(NetPacket* _packet)
{
	auto recv_data = PACKET_TO_FBSTRUCT(_packet, fb::dbagent::Recv_Reg);
	return true;
}
