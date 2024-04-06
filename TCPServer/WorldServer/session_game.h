#pragma once

#include "../Common/session_base.h"
#include "../Core/dispatcher.h"

class SessionGame : public SessionBase
{
public:
	using Pid_t = fb::world::SendPid;
	using Function_t = bool (SessionGame::*)(NetPacket*);

public:
	static bool InitDispatcher();
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionGame(std::size_t _buffer_size);
	virtual ~SessionGame();

	bool RecvPacket(NetPacket* _packet) override; // SessionBase
	void OnError(const std::string& _errmsg) override;
	void OnError(std::exception& _exception) override;
	bool ProcPacket(NetPacket* _packet) override; // JobOwner
	ThreadId_t ThreadId() const override; // JobOwner

protected: // 패킷 처리 함수
	bool OnSend_Reg(NetPacket* _packet);
	bool OnSend_Ping(NetPacket* _packet);
};