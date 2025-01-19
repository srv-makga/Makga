#pragma once

#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"
#include "../Core/iocp_session.h"

class SessionWorld : public core::network::IocpSession, public core::pattern::Singleton<SessionWorld>
{
public:
	using Pid_t = fb::world::RecvPid;
	using Function_t = bool (SessionWorld::*)(NetPacket*);

public:
	static bool InitDispatcher();
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionWorld(SessionId_t _session_id, std::size_t _buffer_size = CONFIG.buffer_size_read);
	virtual ~SessionWorld();

	bool RecvPacket(NetPacket* _packet) override; // SessionBase
	bool ProcPacket(NetPacket* _packet) override; // JobOwner
	ThreadId_t ThreadId() const override; // JobOwner

protected: // ��Ŷ ó�� �Լ�
	bool OnRecv_Reg(NetPacket* _packet);
	bool OnRecv_Ping(NetPacket* _packet);
};

#define SESSION_WORLD	SessionWorld::Instance()