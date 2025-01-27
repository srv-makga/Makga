#pragma once

#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"
#include "../Core/iocp_session.h"

class SessionDBAgent : public core::network::IocpSession, public core::pattern::Singleton<SessionDBAgent>
{
public:
	using Pid_t = fb::dbagent::RecvPid;
	using Function_t = std::function<bool(SessionDBAgent*, std::shared_ptr<Packet>)>;

public:
	static bool InitDispatcher();
private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionDBAgent(std::size_t _buffer_size = 8192);
	virtual ~SessionDBAgent();

public: // IocpSession
	void OnConnected() override;
	void OnDisconnected() override;
	std::size_t OnRecv(char* buffer, std::size_t _length) override;
	bool ProcPacket(std::shared_ptr<Packet> _packet) override;

public:
	bool Send(fb::dbagent::SendPid _pid, fbb& _fbb);

	ThreadId_t ThreadId() const;

	/******************* 패킷 처리 함수 *******************/
	// @brief DBA 서버에 연결 응답
	bool OnReg(std::shared_ptr<NetPacket> _packet);

};

#define SESSION_DBAGENT	SessionDBAgent::Instance()