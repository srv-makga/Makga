#pragma once

#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"
#include "../Core/iocp_session.h"

class SessionDBAgent : public core::network::IocpSession, public core::pattern::Singleton<SessionDBAgent>
{
public:
	using Pid_t = fb::dbagent::RecvPid; // @todo 수정
	using Function_t = bool (SessionDBAgent::*)(NetPacket*);

public:
	static bool InitDispatcher();
private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionDBAgent(std::size_t _buffer_size = 8192);
	virtual ~SessionDBAgent();

public: // IocpSession
	// @brief 패킷 처리
	bool ProcPacket(NetPacket* _packet) override;

	ThreadId_t ThreadId() const;

	/******************* 패킷 처리 함수 *******************/
	// @brief DBA 서버에 연결 응답
	bool OnRecv_Reg(NetPacket* _packet);

};

#define SESSION_DBAGENT	SessionDBAgent::Instance()