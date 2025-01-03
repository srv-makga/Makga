#pragma once

#include "../Core/iocp_session.h"
#include "../Core/object_pool.h"
#include "../Core/dispatcher.h"

class User;

// ���� ���ᰡ �Ǿ �������� ���� ���� ������ ����־ User�� Session�� ���� ��ӹ��� �ʴ´�

class SessionUser : public core::network::IocpSession
{
public:
	using Pid_t = fb::server::SendPid;
	using Function_t = bool (SessionUser::*)(NetPacket*);

public:
	static bool InitDispatcher();

private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionUser(Id _id);
	virtual ~SessionUser();
	 
	bool Initialize() override;
	void Finalize() override;

	bool ProcPacket(NetPacket* packet);
	ThreadId_t ThreadId() const;

protected:  // IocpSession
	void OnConnected() override;
	std::size_t OnRecv(char* buffer, std::size_t _length) override;
	void OnRecvPacket(char* buffer, std::size_t _length) override;
	void OnSend(std::size_t _length) override;
	void OnDisconnected() override;

protected:
	bool OnLoginAuth(NetPacket* _packet);

public:
	User* GetUser() const;
	void SetUser(User* _user);

private:
	User* m_user;
};