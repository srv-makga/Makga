#pragma once

#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"
#include "../Core/iocp_session.h"

class SessionDBAgent : public core::network::IocpSession, public core::pattern::Singleton<SessionDBAgent>
{
public:
	using Pid_t = fb::dbagent::RecvPid; // @todo ����
	using Function_t = bool (SessionDBAgent::*)(NetPacket*);

public:
	static bool InitDispatcher();
private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionDBAgent(std::size_t _buffer_size = 8192);
	virtual ~SessionDBAgent();

public: // IocpSession
	// @brief ��Ŷ ó��
	bool ProcPacket(NetPacket* _packet) override;

	ThreadId_t ThreadId() const;

	/******************* ��Ŷ ó�� �Լ� *******************/
	// @brief DBA ������ ���� ����
	bool OnRecv_Reg(NetPacket* _packet);

};

#define SESSION_DBAGENT	SessionDBAgent::Instance()