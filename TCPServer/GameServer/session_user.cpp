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

SessionUser::SessionUser()
	: SessionBase(core::BufferFlag::None, CONFIG.session_user_buffer_size)
	, m_user(nullptr)
{
}

SessionUser::~SessionUser()
{
}

void SessionUser::Initialize()
{
	SessionBase::Initialize();
	m_user = nullptr;
}

bool SessionUser::RecvPacket(NetPacket* _packet)
{
	Job* job = Job::Pop();
	job->owner = (nullptr == GetUser()) ? (JobOwner*)this : GetUser();
	job->packet = _packet;
	Actor()->JobHandler()->Push(job);

	return true;
}

bool SessionUser::ProcPacket(NetPacket* _packet)
{
	fb::server::SendPid pid = (fb::server::SendPid)_packet->Id();

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

bool SessionUser::OnLoginAuth(NetPacket* _packet)
{
	return false;
}