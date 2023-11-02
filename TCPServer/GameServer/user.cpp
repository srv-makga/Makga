#include "pch.h"
#include "user.h"
#include "session_user.h"
#include "server_game.h"
#include "inventory_account.h"

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
	return Inventory(_item_uid)->FindObject(_item_uid);
}

ItemObjectBase* User::FindItemObject(ItemIdx_t _item_idx)
{
	return Inventory(_item_idx)->FindObject(_item_idx);
}

InventoryBase* User::Inventory(ItemUid_t _item_uid) const
{
	return m_inventory;
}

InventoryBase* User::Inventory(ItemIdx_t _item_idx) const
{
	// @todo 아이템별 인벤토리 반환
	return m_inventory;
}
