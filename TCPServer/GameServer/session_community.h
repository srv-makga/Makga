#pragma once

#include "../Common/session_base.h"
#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"

class ConnectorBase;

class CommunityServer : public SessionBase, public core::pattern::Singleton<CommunityServer>
{
public:
	using Pid_t = fb::community::RecvPid; // @todo 수정
	using Function_t = bool (CommunityServer::*)(NetPacket*);

public:
	static bool InitDispatcher();

public:
	CommunityServer();
	virtual ~CommunityServer();

	// @brief 패킷 수신시
	bool RecvPacket(NetPacket* _packet) override;
	// @brief 패킷 처리
	bool ProcPacket(NetPacket* _packet) override;

	ThreadId_t ThreadId() const override;

	/******************* 패킷 처리 함수 *******************/
	// @brief 월드 서버에 연결 응답
	bool OnReg(NetPacket* _packet);

private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;
};

// @todo 추후 여러개의 커뮤니티가 생길 경우 Proxy 객체를 만들고 그 객체를 define 한다.
// 내부에선 uid등 규칙에 따라 분배한다
#define COMMUNITY_SERVER	CommunityServer::Instance()