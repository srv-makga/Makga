#pragma once

#include "../Core/dispatcher.h"
#include "app_session.h"

class CommunitySession : public AppSession
{
public:
	using Pid_t = fb::community::RecvPid;
	using Function_t = std::function<bool(CommunitySession*, std::shared_ptr<Packet>)>;

public:
	static bool InitDispatcher();
private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	CommunitySession(std::size_t _buffer_size);
	virtual ~CommunitySession();

public: // AppSession
	bool Initialize() override;
	void Finalize() override;

	bool ProcPacket(std::shared_ptr<NetPacket> _packet) override;
	ThreadId_t ThreadId() const override;

protected:
	bool OnReg(std::shared_ptr<NetPacket> _packet);
};

// @todo 추후 여러개의 커뮤니티가 생길 경우 Proxy 객체를 만들고 그 객체를 define 한다.
// 내부에선 uid등 규칙에 따라 분배한다
#define COMMUNITY_SERVER	CommunityServer::Instance()