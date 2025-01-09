#include "pch.h"
#include "session_user.h"
#include "user.h"

#define REG_DISPATCHER(pid)	s_dispatcher.Add(fb::server::SendPid_##pid, &SessionUser::On##pid);

core::Dispatcher<SessionUser::Pid_t, SessionUser::Function_t> SessionUser::s_dispatcher;

bool SessionUser::InitDispatcher()
{
	REG_DISPATCHER(LoginAuth);
	
	return true;
}

SessionUser::SessionUser(Id _id)
	: IocpSession(CONFIG.buffer_size_user)
	, m_user(nullptr)
{
}

SessionUser::~SessionUser()
{
}

bool SessionUser::Initialize()
{
	m_user = nullptr;

	Session::Initialize();

	return true;
}

void SessionUser::Finalize()
{
	Session::Finalize();
}

bool SessionUser::ProcPacket(NetPacket* _packet)
{
	fb::server::SendPid pid = static_cast<fb::server::SendPid>(_packet->GetId());

	LOG_INFO << "ProcPacket. Pid:" << fb::server::EnumNameSendPid(pid);

	bool ret = s_dispatcher.Exec(pid, this, _packet);
	LOG_ERROR_IF(false == ret) << "Fail to dispatcher exec. Pid:" << fb::server::EnumNameSendPid(pid);

	return ret;
}

User* SessionUser::GetUser() const
{
	return m_user;
}

void SessionUser::SetUser(User* _user)
{
	if (m_user == _user)
	{
		return;
	}

	if (nullptr != m_user)
	{
		m_user->SetSession(nullptr);
	}

	m_user = _user;

	if (nullptr != _user)
	{
		_user->SetSession(this);
	}
}

ThreadId_t SessionUser::ThreadId() const
{
	return 0;
}

void SessionUser::OnConnected()
{
	
}

std::size_t SessionUser::OnRecv(char* buffer, std::size_t _length)
{
	NetPacket* packet;
	return std::size_t();
}

void SessionUser::OnSend(std::size_t _length)
{
}

void SessionUser::OnDisconnected()
{
}

bool SessionUser::OnLoginAuth(NetPacket* _packet)
{
	return false;
}