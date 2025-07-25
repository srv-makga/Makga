#pragma once

#include "../Core/dispatcher.h"
#include "app_session.h"

class DBAgentSession : public AppSession
{
public:
	using Pid_t = fb::dbagent::RecvPid;
	using Function_t = std::function<bool(DBAgentSession*, std::shared_ptr<Packet>)>;

public:
	static bool InitDispatcher();
private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	DBAgentSession(std::size_t _buffer_size);
	virtual ~DBAgentSession();

public: // AppSession
	bool Initialize() override;
	void Finalize() override;

	bool ProcPacket(std::shared_ptr<NetPacket> _packet) override;
	ThreadId_t ThreadId() const override;

protected:
	bool OnReg(std::shared_ptr<NetPacket> _packet);
};