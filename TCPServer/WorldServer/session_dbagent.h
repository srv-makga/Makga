#pragma once

#include "../Common/session_base.h"
#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"

class SessionDBAgent : public SessionBase, public core::pattern::Singleton<SessionDBAgent>
{
public:
	using Pid_t = fb::dbagent::RecvPid;
	using Function_t = bool (SessionDBAgent::*)(NetPacket*);

public:
	static bool InitDispatcher();
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionDBAgent(std::size_t _buffer_size = CONFIG.buffer_size_read);
	virtual ~SessionDBAgent();

	bool RecvPacket(NetPacket* _packet) override; // SessionBase
	void OnError(const std::string& _errmsg) override; // SessionBase
	void OnError(std::exception& _exception) override; // SessionBase

	bool ProcPacket(NetPacket* _packet) override; // JobOwner
	ThreadId_t ThreadId() const override; // JobOwner

protected: // 패킷 처리 함수
	bool OnRecv_Reg(NetPacket* _packet);
	bool OnRecv_Ping(NetPacket* _packet);
};

#define SESSION_DBAGENT	SessionDBAgent::Instance()