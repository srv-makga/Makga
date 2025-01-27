#pragma once

#include "../Core/dispatcher.h"
#include "../Core/singleton.hpp"
#include "../Core/iocp_session.h"

class CommunityServer : public core::network::IocpSession, public core::pattern::Singleton<CommunityServer>
{
public:
	using Pid_t = fb::community::RecvPid;
	using Function_t = std::function<bool(CommunityServer*, std::shared_ptr<Packet>)>;

public:
	static bool InitDispatcher();
private:
	inline static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	CommunityServer();
	virtual ~CommunityServer();

public: // IocpSession
	void OnConnected() override;
	void OnDisconnected() override;
	std::size_t OnRecv(char* buffer, std::size_t _length) override;
	bool ProcPacket(std::shared_ptr<Packet> _packet) override;

public:
	ThreadId_t ThreadId() const;

	/******************* ��Ŷ ó�� �Լ� *******************/
	// @brief ���� ������ ���� ����
	bool OnReg(std::shared_ptr<NetPacket> _packet);
};

// @todo ���� �������� Ŀ�´�Ƽ�� ���� ��� Proxy ��ü�� ����� �� ��ü�� define �Ѵ�.
// ���ο��� uid�� ��Ģ�� ���� �й��Ѵ�
#define COMMUNITY_SERVER	CommunityServer::Instance()