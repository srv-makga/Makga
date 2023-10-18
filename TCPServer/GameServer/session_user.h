#pragma once

#include "../Common/session_base.h"
#include "../Core/object_pool.h"
#include "../Core/dispatcher.h"

class User;

// ���� ���ᰡ �Ǿ �������� ���� ���� ������ ����־ User�� Session�� ���� ��ӹ��� �ʴ´�

class SessionUser : public SessionBase
{
public:
	using Pid_t = fb::server::SendPid;
	using Function_t = bool (SessionUser::*)(NetPacket*);

public:
	static bool InitDispatcher();

private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionUser();
	virtual ~SessionUser();

	void Initialize();

	bool RecvPacket(NetPacket* packet) override;
	bool ProcPacket(NetPacket* packet) override;
	ThreadId_t ThreadId() const override;

protected:
	bool OnLoginAuth(NetPacket* _packet);

public:
	User* GetUser() const;
	void SetUser(User* _user);

private:
	User* m_user;
};