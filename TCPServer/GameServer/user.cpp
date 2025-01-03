#include "pch.h"
#include "user.h"
#include "session_user.h"
#include "server_game.h"
#include "inventory_user.h"
#include "item_object_base.h"

User::User()
{
}

User::~User()
{
}

void User::Initialize()
{
	m_account.Clear();
	m_auth_key.Clear();
	m_session = nullptr;
	m_max_inventory = 0;
	m_user_uid = 0;
}

void User::Finalize()
{
}

void User::Move(const Vector_t& _vec, Speed_t _speed, int _effect, int _animation)
{
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

bool User::Send(fb::server::RecvPid _pid, fbb& _fbb)
{
	if (nullptr == m_session)
	{
		LOG_ERROR << "m_session is nullptr" << LOG_USER(this);
		return false;
	}

	NetPacket packet;
	packet.SetOwner(m_session);
	m_session->Send(packet);
	return true;
}

Character* User::ActiveCharacter() const
{
	return m_character;
}

void User::SetActiveCharacter(Character* _character)
{
	m_character = _character;
}