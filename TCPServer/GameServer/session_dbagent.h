#pragma once

#include "connector_dbagent.h"
#include "../Common/session_base.h"
#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"

class SessionDBAgent : public SessionBase, public core::pattern::Singleton<SessionDBAgent>
{
public:
	using Pid_t = fb::dbagent::RecvPid; // @todo 수정
	using Function_t = bool (SessionDBAgent::*)(NetPacket*);

public:
	static bool InitDispatcher();

public:
	SessionDBAgent(std::size_t _buffer_size);
	virtual ~SessionDBAgent();

	// @brief 패킷 수신시
	bool RecvPacket(NetPacket* _packet) override;
	// @brief 패킷 처리
	bool ProcPacket(NetPacket* _packet) override;

	ThreadId_t ThreadId() const override;

	/******************* 패킷 처리 함수 *******************/
	// @brief DBA 서버에 연결 응답
	bool OnRecv_Reg(NetPacket* _packet);

private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;
};

#define SESSION_DBAGENT	SessionDBAgent::Instance()