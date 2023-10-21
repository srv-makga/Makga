#include "pch.h"
#include "user.h"
#include "session_user.h"

User::User()
{
}

User::~User()
{
}

void User::Initialize()
{
	m_user_uid = 0;
}

void User::Finalize()
{
	m_user_uid = 0;
}

const String8& User::Account() const
{
	return m_account;
}

SessionUser* User::Session() const
{
	return m_session;
}

void User::SetSession(SessionUser* _session)
{
	m_session = _session;
}

bool User::ProcPacket(NetPacket* _packet)
{
	fb::server::SendPid pid = (fb::server::SendPid)_packet->Id();

	LOG_INFO << "ProcPacket. Pid:" << fb::server::EnumNameSendPid(pid);

	bool ret = s_dispatcher.Exec(pid, this, _packet);
	LOG_ERROR_IF(false == ret) << "Fail to dispatcher exec. Pid:" << fb::server::EnumNameSendPid(pid);

	return ret;
}