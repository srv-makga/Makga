#pragma once

#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"
#include "../Core/iocp_session.h"

class CommunityServer : public core::network::IocpSession, public core::pattern::Singleton<CommunityServer>
{
public:
	using Pid_t = fb::community::RecvPid; // @todo ����
	using Function_t = bool (CommunityServer::*)(NetPacket*);

public:
	static bool InitDispatcher();
private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	CommunityServer();
	virtual ~CommunityServer();

public: // IocpSession
	// @brief ��Ŷ ó��
	bool ProcPacket(Packet* _packet) override;

public:
	ThreadId_t ThreadId() const;

	/******************* ��Ŷ ó�� �Լ� *******************/
	// @brief ���� ������ ���� ����
	bool OnReg(NetPacket* _packet);
};

// @todo ���� �������� Ŀ�´�Ƽ�� ���� ��� Proxy ��ü�� ����� �� ��ü�� define �Ѵ�.
// ���ο��� uid�� ��Ģ�� ���� �й��Ѵ�
#define COMMUNITY_SERVER	CommunityServer::Instance()