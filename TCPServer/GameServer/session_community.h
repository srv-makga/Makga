#pragma once

#include "../Common/session_base.h"
#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"

class ConnectorBase;

class CommunityServer : public SessionBase, public core::pattern::Singleton<CommunityServer>
{
public:
	using Pid_t = fb::community::RecvPid; // @todo ����
	using Function_t = bool (CommunityServer::*)(NetPacket*);

public:
	static bool InitDispatcher();

public:
	CommunityServer();
	virtual ~CommunityServer();

	// @brief ��Ŷ ���Ž�
	bool RecvPacket(NetPacket* _packet) override;
	// @brief ��Ŷ ó��
	bool ProcPacket(NetPacket* _packet) override;

	ThreadId_t ThreadId() const override;

	/******************* ��Ŷ ó�� �Լ� *******************/
	// @brief ���� ������ ���� ����
	bool OnReg(NetPacket* _packet);

private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;
};

// @todo ���� �������� Ŀ�´�Ƽ�� ���� ��� Proxy ��ü�� ����� �� ��ü�� define �Ѵ�.
// ���ο��� uid�� ��Ģ�� ���� �й��Ѵ�
#define COMMUNITY_SERVER	CommunityServer::Instance()