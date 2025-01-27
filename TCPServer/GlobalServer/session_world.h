#pragma once

#include "../Core/object_pool.h"
#include "../Core/dispatcher.h"
#include "../Core/iocp_session.h"

class SessionWorld : public core::network::IocpSession
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

	bool Initialize();

public: // IocpSession
	bool ProcPacket(NetPacket* packet) override;

public:
	ThreadId_t ThreadId() const;

protected:
	bool OnRegServer(NetPacket* _packet);
};