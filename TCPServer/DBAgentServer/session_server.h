#pragma once

#include "../Common/session_base.h"
#include "../Core/object_pool.h"
#include "../Core/dispatcher.h"

class SessionServer : public SessionBase
{
public:
	using Pid_t = fb::dbagent::SendPid;
	using Function_t = bool (SessionServer::*)(NetPacket*);

public:
	static bool InitDispatcher();
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionServer(std::size_t _buffer_size);
	virtual ~SessionServer();

	bool RecvPacket(NetPacket* _packet) override; // SessionBase
	bool ProcPacket(NetPacket* _packet) override; // JobOwner
	ThreadId_t ThreadId() const override; // JobOwner

protected: // 패킷 처리 함수
	bool OnReg(NetPacket* _packet);

};