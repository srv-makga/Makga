#pragma once

#include "game_header.h"
#include "../Core/object_pool.h"
#include "../Common/session_base.h"
#include "../Common/inventory_owner.h"
#include "../Core/dispatcher.h"

class SessionUser;

class User : public core::ObjectPool<User*>, public JobOwner, public InventoryOwner
{
public:
	using Pid_t = fb::server::SendPid;
	using Function_t = bool (User::*)(NetPacket*);

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

	SessionUser* Session() const;
	void SetSession(SessionUser* _session);

public: // JobOwner
	bool ProcPacket(NetPacket* _packet) override;
	ThreadId_t ThreadId() const override { return 0; }

public:
	bool OnLoginSecurity(NetPacket* _packet);
	bool OnCharacterCreate(NetPacket* _packet);
	bool OnCharacterDelete(NetPacket* _packet);
	bool OnCharacterSelect(NetPacket* _packet);
	bool OnCharacterLogout(NetPacket* _packet);

public: // InventoryOwner
	uint64_t OwnerUid() const { return m_user_uid; }
	int32_t MaxInvenSlot() const { return m_max_inventory; }

private:
	String8 m_account;
	String8 m_auth_key;

	SessionUser* m_session;

	Count_t m_max_inventory;

	UserUid_t m_user_uid;
}; 