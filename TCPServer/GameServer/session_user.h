#pragma once

#include "../Common/session_base.h"
#include "../Core/object_pool.h"
#include "../Core/dispatcher.h"

class User;

// 접속 종료가 되어도 재접속을 위해 유저 정보는 살아있어서 User가 Session을 직접 상속받지 않는다

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