#pragma once

#include "../Core/iocp_session.h"
#include "../Core/object_pool.h"
#include "../Core/dispatcher.h"

class User;

// 접속 종료가 되어도 재접속을 위해 유저 정보는 살아있어서 User가 Session을 직접 상속받지 않는다

class SessionUser : public core::network::IocpSession
{
public:
	using Pid_t = fb::server::SendPid;
	using Function_t = std::function<bool(SessionUser*, std::shared_ptr<Packet>)>;

public:
	static bool InitDispatcher();

private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	SessionUser(std::size_t _buffer_size);
	virtual ~SessionUser();
	 
	bool Initialize() override;
	void Finalize() override;

	ThreadId_t ThreadId() const;

protected:  // IocpSession
	void ProcConnect() override;
	void ProcDisconnect() override;
	std::size_t ProcRecv(char* _data, std::size_t _recv_size) override;
	void ProcSend(std::size_t _sent_size) override;

protected:
	void ProcPacket(std::shared_ptr<NetPacket> _packet);
	bool OnLoginAuth(std::shared_ptr<NetPacket> _packet);

public:
	User* GetUser() const;
	void SetUser(User* _user);

private:
	User* m_user;
};