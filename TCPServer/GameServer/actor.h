#pragma once

#include "game_header.h"
#include "../Core/object_pool.h"

struct stHpMp
{
	Hp_t max_hp;
	Hp_t cur_hp;
	Mp_t max_mp;
	Mp_t cur_mp;
};

class Character;
class Monster;
class Npc;
class Gadget;

/*
* @brief 맵에서 보일 객체의 인터페이스
*/
class Actor
{
public:
	Actor() = default;
	virtual ~Actor() = default;

	virtual void Initialize() = 0;
	virtual void Finallize() = 0;

	virtual void OnUpdate() = 0;

	virtual fb::eActorType Type() const { return fb::eActorType_None; }
	virtual bool IsCharacter() const { return false; }
	virtual bool IsMonster() const { return false; }
	virtual bool IsNpc() const { return false; }
	virtual bool IsGadget() const { return false; }

	// @brief 공격할 수 있는
	virtual bool IsAttackable() const { return false; }
	// @brief 공격받 수 있는
	virtual bool IsAttacked() const { return false; }
	// @brief 이동이 가능한
	virtual bool IsMovable() const { return false; }
	// @brief 누가 주변에 올 때 appear 정보를 줄지
	virtual bool IsAppear() const { return false; }

	virtual bool HasTarget() const { return false; }
	virtual Actor* Target() const { return nullptr; }
	virtual bool IsDie() const { return true; }

	virtual Coord_t X() const { return Coord_t(); }
	virtual Coord_t Y() const { return Coord_t(); }
	virtual Coord_t Z() const { return Coord_t(); }

	virtual Hp_t MaxHp() const { return 0; }
	virtual Hp_t CurHp() const { return 0; }
	virtual Mp_t MaxMp() const { return 0; }
	virtual Mp_t CurMp() const { return 0; }

public:
	ActorId_t Id() const { return m_id; }
	void SetId(ActorId_t _id) { m_id = _id; }

protected:
	ActorId_t m_id;
};