#include "pch.h"
#include "user.h"
#include "session_user.h"
#include "server_game.h"
#include "inventory_account.h"
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

	return m_session->Send(_pid, _fbb);
}

StackCount_t User::ItemCount(ItemIdx_t _item_idx) const
{
	auto item_object = m_inventory->FindObject(g_gold_idx);
	return item_object ? item_object->Stack() : 0;
}

Currency_t User::Bronze() const
{
	return ItemCount(g_bronze_idx);
}

Currency_t User::Silver() const
{
	return ItemCount(g_silver_idx);
}

Currency_t User::Gold() const
{
	return ItemCount(g_gold_idx);
}

Character* User::GetCharacter() const
{
	return m_character;
}

void User::SetCharacter(Character* _character)
{
	m_character = _character;
}

ItemObjectBase* User::FindItemObject(ItemUid_t _item_uid)
{
	return m_inventory->FindObject(_item_uid);
}

ItemObjectBase* User::FindItemObject(ItemIdx_t _item_idx)
{
	return m_inventory->FindObject(_item_idx);
}

void User::Reflection(ItemUid_t _item_uid, ReflectType _type, bool _is_send_client)
{
}

InventoryBase* User::Inventory() const
{
	return m_inventory;
}