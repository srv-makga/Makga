#pragma once

#include "../Core/iocp_session.h"
#include "../Core/object_pool.h"
#include "../Core/dispatcher.h"

class SessionAdmintool : public core::network::IocpSession
{
public:
	using Pid_t = fb::server::SendPid;
	using Function_t = std::function<bool(SessionAdmintool*, std::shared_ptr<Packet>)>;

public:
	static bool InitDispatcher();

private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionAdmintool();
	virtual ~SessionAdmintool();

	bool Initialize() override;
	void Finalize() override;

	ThreadId_t ThreadId() const;

protected:  // IocpSession
	void OnConnected() override;
	void OnDisconnected() override;
	std::size_t OnRecv(char* _buffer, std::size_t _length) override;
	bool ProcPacket(std::shared_ptr<NetPacket> _packet) override;

protected:
	//bool OnLoginAuth(std::shared_ptr<NetPacket> _packet);
};