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
private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionWorld(std::size_t _buffer_size = CONFIG.buffer_size_read);
	virtual ~SessionWorld();

public:
	bool ProcPacket(NetPacket* _packet) override; // IocpSession

public:
	ThreadId_t ThreadId() const; // JobOwner

protected: // 패킷 처리 함수
	bool OnRecv_Reg(NetPacket* _packet);
	bool OnRecv_Ping(NetPacket* _packet);
};

#define SESSION_WORLD	SessionWorld::Instance()