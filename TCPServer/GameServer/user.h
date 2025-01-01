#pragma once

#include "game_header.h"
#include "../Core/object_pool.h"
#include "../Common/inventory_owner.h"
#include "../Core/dispatcher.h"

class SessionUser;
class Character;
class ItemObjectBase;
class InventoryBase;
class InventoryUser;

class User : public core::ObjectPool<User*>, public JobOwner//, public InventoryOwner
{
public:
	using Pid_t = fb::server::SendPid;
	using Function_t = bool (User::*)(NetPacket*);
	using InventoryCache = std::unordered_map<ItemUid_t, InventoryBase*>;
	using UserState = enum { Lobby, Play };

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

	std::atomic<UserState> state;
	UserState State() const { return state; }
	void SetState(UserState _state)  { state = _state; }

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

	UserUid_t m_user_uid;

	SessionUser* m_session;

	Count_t m_max_inventory;

	Character* m_character;

	uint32_t m_slot; // 몇번째 슬롯 캐릭터인지

	GuildUid_t m_guild_uid;
	String8 m_guild_name;
	bool m_is_guild_leader;

	// interaction
	ActorUid_t m_interaction_id;
	Time_t m_interaction_expire;

	InventoryCache m_inventory_cache;
	InventoryUser* m_inventory;
	InventoryUser* m_warehouse;
}; 