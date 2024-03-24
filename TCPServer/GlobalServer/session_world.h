#pragma once

#include "../Common/session_base.h"
#include "../Core/object_pool.h"
#include "../Core/dispatcher.h"

class SessionWorld : public SessionBase
{
public:
	using Pid_t = fb::server::SendPid;
	using Function_t = bool (SessionWorld::*)(NetPacket*);

public:
	static bool InitDispatcher();

private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionWorld(std::size_t _buffer_size);
	virtual ~SessionWorld();

	void Initialize();

	bool RecvPacket(NetPacket* packet) override;
	bool ProcPacket(NetPacket* packet) override;
	ThreadId_t ThreadId() const override;

protected:
	bool OnRegServer(NetPacket* _packet);
};