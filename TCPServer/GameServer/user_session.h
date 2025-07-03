#pragma once

#include "../Core/object_pool.h"
#include "../Core/dispatcher.h"
#include "app_session.h"

class User;

// ���� ���ᰡ �Ǿ �������� ���� ���� ������ ����־ User�� Session�� ���� ��ӹ��� �ʴ´�

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
	 
	bool Initialize() override;
	void Finalize() override;

	ThreadId_t ThreadId() const;

protected: // AppSession
	void ProcPacket(std::shared_ptr<NetPacket> _packet) override;

protected:
	bool OnLoginAuth(std::shared_ptr<NetPacket> _packet);

public:
	User* GetUser() const;
	void SetUser(User* _user);

private:
	User* m_user;
};