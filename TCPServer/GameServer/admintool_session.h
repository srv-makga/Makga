#pragma once

#include "../Core/dispatcher.h"
#include "app_session.h"

class AdmintoolSession : public AppSession
{
public:
	using Pid_t = fb::server::SendPid;
	using Function_t = std::function<bool(AdmintoolSession*, std::shared_ptr<Packet>)>;
	using Session_t = core::network::IocpSession;

public:
	static bool InitDispatcher();

private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	AdmintoolSession(std::size_t _buffer_size);
	virtual ~AdmintoolSession();

public: // AppSession
	bool Initialize() override;
	void Finalize() override;

	bool ProcPacket(std::shared_ptr<NetPacket> _packet) override;
	ThreadId_t ThreadId() const;

protected:
	//bool OnLoginAuth(std::shared_ptr<NetPacket> _packet);
};