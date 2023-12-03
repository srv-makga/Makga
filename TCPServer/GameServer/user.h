#pragma once

#include "game_header.h"
#include "../Core/object_pool.h"
#include "../Common/session_base.h"
#include "../Common/inventory_owner.h"
#include "../Core/dispatcher.h"

class SessionUser;
class Character;
class ItemObjectBase;
class InventoryBase;
class InventoryAccount;

class User : public core::ObjectPool<User*>, public JobOwner//, public InventoryOwner
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
	void Move(const Vector_t& _vec, Speed_t _speed, int _effect, int _animation);

public: // JobOwner
	bool ProcPacket(NetPacket* _packet) override;
	ThreadId_t ThreadId() const override { return 0; }

public: // Session 랩핑 함수
	bool Send(fb::server::RecvPid _pid, fbb& _fbb);

public: // 멤버 변수 get/set
	const String8& Account() const;

	UserUid_t UserUid() const { return m_user_uid; }

	SessionUser* Session() const;
	void SetSession(SessionUser* _session);

	Character* ActiveCharacter() const;
	void SetActiveCharacter(Character* _character);

public: // 패킷 처리 함수
	bool OnChatting(NetPacket* _packet);
	bool OnLoginSecurity(NetPacket* _packet);
	bool OnCharacterCreate(NetPacket* _packet);
	bool OnCharacterDelete(NetPacket* _packet);
	bool OnCharacterSelect(NetPacket* _packet);
	bool OnCharacterLogout(NetPacket* _packet);
	bool OnCharacterMove(NetPacket* _packet);
	bool OnCharacterAngle(NetPacket* _packet);

private:
	String8 m_account;
	String8 m_auth_key;

	SessionUser* m_session;

	Count_t m_max_inventory;

	UserUid_t m_user_uid;

	Character* m_character;

	// interaction
	ActorUid_t m_interaction_id;
	Time_t m_interaction_expire;

	InventoryCache m_inventory_cache;
	InventoryAccount* m_inventory;
}; 