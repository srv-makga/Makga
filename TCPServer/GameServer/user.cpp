#include "pch.h"
#include "user.h"
#include "user_session.h"
#include "server_game.h"
#include "pool.h"
#include "inventory_user.h"
#include "item_object_base.h"

ThreadId_t User::ThreadId() const
{
	return ThreadId_t();
}

User::User()
	: m_user_uid(0)
	, m_session(nullptr)
	, m_max_inventory(0)
	, m_character(nullptr)
	, m_slot(0)
	, m_guild_uid(0)
	, m_is_guild_leader(false)
	, m_interaction_id(0)
	, m_interaction_expire(0)
	, m_inventory(nullptr)
	, m_warehouse(nullptr)
{
}

User::~User()
{
	m_session = nullptr;
	m_character = nullptr;
	m_inventory = nullptr;
	m_warehouse = nullptr;
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
	m_account.Clear();
	m_auth_key.Clear();
	m_session = nullptr;
	m_max_inventory = 0;
	m_user_uid = 0;
}

void User::Move(const Vector_t& _vec, Speed_t _speed, int _effect, int _animation)
{
}

const String8& User::Account() const
{
	return m_account;
}

UserSession* User::Session() const
{
	return m_session;
}

void User::SetSession(UserSession* _session)
{
	m_session = _session;
}

bool User::Send(fb::server::RecvPid _pid, fbb& _fbb)
{
	if (nullptr == m_session)
	{
		LOG_ERROR << "m_session is nullptr" << LOG_USER(this);
		return false;
	}

	m_session->Send(_pid, _fbb);

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