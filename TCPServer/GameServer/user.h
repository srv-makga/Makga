#pragma once

#include "game_header.h"
#include "../Core/object_pool.h"
#include "../Common/session_base.h"
#include "../Common/inventory_owner.h"
#include "../Core/dispatcher.h"

class SessionUser;
class Character;
class InventoryBase;
class InventoryAccount;

class User : public core::ObjectPool<User*>, public JobOwner, public InventoryOwner
{
public:
	using Pid_t = fb::server::SendPid;
	using Function_t = bool (User::*)(NetPacket*);
	using InventoryCache = std::unordered_map<ItemUid_t, InventoryBase*>;

public:
	static bool InitDispatcher();

private:
	static core::Dispatcher<Pid_t, Function_t> s_dispatcher;

public:
	User();
	virtual ~User();

	void Initialize();
	void Finalize();

public:
	const String8& Account() const;

	UserUid_t UserUid() const { return m_user_uid; }

	SessionUser* Session() const;
	void SetSession(SessionUser* _session);

public: // JobOwner
	bool ProcPacket(NetPacket* _packet) override;
	ThreadId_t ThreadId() const override { return 0; }

public: // Session ·¦ÇÎ ÇÔ¼ö
	bool Send(fb::server::RecvPid _pid, fbb& _fbb);

public:
	bool OnLoginSecurity(NetPacket* _packet);
	bool OnCharacterCreate(NetPacket* _packet);
	bool OnCharacterDelete(NetPacket* _packet);
	bool OnCharacterSelect(NetPacket* _packet);
	bool OnCharacterLogout(NetPacket* _packet);
	bool OnCharacterMove(NetPacket* _packet);
	bool OnCharacterAngle(NetPacket* _packet);
	bool OnCharacterResurrection(NetPacket* _packet);
	bool OnActorInteractionStart(NetPacket* _packet);
	bool OnActorInteractionEnd(NetPacket* _packet);
	bool OnActorInteractionCancel(NetPacket* _packet);
	bool OnItemDestroy(NetPacket* _packet);
	bool OnItemUse(NetPacket* _packet);
	bool OnItemMake(NetPacket* _packet);
	bool OnItemReinforce(NetPacket* _packet);
	bool OnItemDisassemble(NetPacket* _packet);
	bool OnItemEnchant(NetPacket* _packet);
	bool OnItemSkinChange(NetPacket* _packet);

public: // InventoryOwner
	uint64_t OwnerUid() const override { return m_user_uid; }
	int32_t MaxInvenSlot() const override { return m_max_inventory; }

	virtual eResult GiveItem(ItemIdx_t _item_idx, StackCount_t _item_stack, bool _is_send_client) = 0;
	virtual eResult GiveItem(const ItemProperty& _item_property, StackCount_t _item_stack, bool _is_send_client) = 0;
	virtual eResult GiveItem(UmapItemList& _item_list, bool _is_send_client) = 0;
	virtual eResult GiveItem(UmapItemPropertyList& _item_list, bool _is_send_client) = 0;

	StackCount_t ItemCount(ItemIdx_t _item_idx) const;

public:
	Currency_t Bronze() const;
	Currency_t Silver() const;
	Currency_t Gold() const;

public:
	Character* GetCharacter() const;
	void SetCharacter(Character* _character);

	ItemObjectBase* FindItemObject(ItemUid_t _item_uid);
	ItemObjectBase* FindItemObject(ItemIdx_t _item_idx);

	void Reflection(ItemUid_t _item_uid, ReflectType _type, bool _is_send_client);

protected:
	InventoryBase* Inventory(const ItemProperty& _property) const;

private:
	String8 m_account;
	String8 m_auth_key;

	SessionUser* m_session;

	Count_t m_max_inventory;

	UserUid_t m_user_uid;

	Character* m_character;

	// interaction
	ActorId_t m_interaction_id;
	Time_t m_interaction_expire;

	InventoryCache m_inventory_cache;
	InventoryAccount* m_inventory;
}; 