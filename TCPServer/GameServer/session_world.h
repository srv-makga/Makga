#pragma once

#include "../Core/dispatcher.h"
#include "../Core/iocp_session.h"

class SessionWorld : public core::network::IocpSession
{
public:
	using Pid_t = fb::world::RecvPid;
	using Function_t = std::function<bool(SessionWorld*, std::shared_ptr<Packet>)>;

public:
	static bool InitDispatcher();
private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionWorld(std::size_t _buffer_size = CONFIG.buffer_size_read);
	virtual ~SessionWorld();

public: // IocpSession
	void OnConnected() override;
	void OnDisconnected() override;
	std::size_t OnRecv(char* buffer, std::size_t _length) override;
	bool ProcPacket(std::shared_ptr<Packet> _packet) override;

public:
	ThreadId_t ThreadId() const; // JobOwner

protected: // 패킷 처리 함수
	bool OnReg(std::shared_ptr<NetPacket> _packet);
	bool OnPing(std::shared_ptr<NetPacket> _packet);
};

#define SESSION_WORLD	SessionWorld::Instance()