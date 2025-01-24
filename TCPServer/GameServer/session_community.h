#pragma once

#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"
#include "../Core/iocp_session.h"

class CommunityServer : public core::network::IocpSession, public core::pattern::Singleton<CommunityServer>
{
public:
	using Pid_t = fb::community::RecvPid; // @todo 수정
	using Function_t = bool (CommunityServer::*)(NetPacket*);

public:
	static bool InitDispatcher();
private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	CommunityServer();
	virtual ~CommunityServer();

public: // IocpSession
	// @brief 패킷 처리
	bool ProcPacket(Packet* _packet) override;

public:
	ThreadId_t ThreadId() const;

	/******************* 패킷 처리 함수 *******************/
	// @brief 월드 서버에 연결 응답
	bool OnReg(NetPacket* _packet);
};

// @todo 추후 여러개의 커뮤니티가 생길 경우 Proxy 객체를 만들고 그 객체를 define 한다.
// 내부에선 uid등 규칙에 따라 분배한다
#define COMMUNITY_SERVER	CommunityServer::Instance()