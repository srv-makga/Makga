#pragma once

#include "../Core/object_pool.h"
#include "../Core/dispatcher.h"
#include "app_session.h"

class User;

// 접속 종료가 되어도 재접속을 위해 유저 정보는 살아있어서 User가 Session을 직접 상속받지 않는다

class UserSession : public AppSession
{
public:
	using Pid_t = fb::server::SendPid;
	using Function_t = std::function<bool(UserSession*, std::shared_ptr<Packet>)>;

public:
	static bool InitDispatcher();

private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	UserSession(std::size_t _buffer_size);
	virtual ~UserSession();
	 
public: // AppSession
	bool Initialize() override;
	void Finalize() override;

	bool ProcPacket(std::shared_ptr<NetPacket> _packet) override;
	ThreadId_t ThreadId() const override;

protected:
	bool OnLoginAuth(std::shared_ptr<NetPacket> _packet);

public:
	User* GetUser() const;
	void SetUser(User* _user);

private:
	User* m_user;
};