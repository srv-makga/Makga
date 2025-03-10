#pragma once

#include "../Core/dispatcher.h"
#include "../Core/iocp_session.h"

class SessionCommunity : public core::network::IocpSession
{
public:
	using Pid_t = fb::community::RecvPid;
	using Function_t = std::function<bool(SessionCommunity*, std::shared_ptr<Packet>)>;

public:
	static bool InitDispatcher();
private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionCommunity();
	virtual ~SessionCommunity();

public: // IocpSession
	void OnConnected() override;
	void OnDisconnected() override;
	std::size_t OnRecv(char* buffer, std::size_t _length) override;
	bool ProcPacket(std::shared_ptr<Packet> _packet) override;

public:
	ThreadId_t ThreadId() const;

	/******************* 패킷 처리 함수 *******************/
	// @brief 월드 서버에 연결 응답
	bool OnReg(std::shared_ptr<NetPacket> _packet);
};

// @todo 추후 여러개의 커뮤니티가 생길 경우 Proxy 객체를 만들고 그 객체를 define 한다.
// 내부에선 uid등 규칙에 따라 분배한다
#define COMMUNITY_SERVER	CommunityServer::Instance()