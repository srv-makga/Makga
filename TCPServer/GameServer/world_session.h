#pragma once

#include "../Core/dispatcher.h"
#include "app_session.h"

class WorldSession : public AppSession
{
public:
	using Pid_t = fb::world::RecvPid;
	using Function_t = std::function<bool(WorldSession*, std::shared_ptr<Packet>)>;

public:
	static bool InitDispatcher();
private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	WorldSession(std::size_t _buffer_size);
	virtual ~WorldSession();

	bool Initialize() override;
	void Finalize() override;

protected: // AppSession
	bool ProcPacket(std::shared_ptr<NetPacket> _packet);
	ThreadId_t ThreadId() const override;

protected:
	bool OnReg(std::shared_ptr<NetPacket> _packet);
	bool OnPing(std::shared_ptr<NetPacket> _packet);
};

#define SESSION_WORLD	WorldSession::Instance()