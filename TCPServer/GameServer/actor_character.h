#pragma once

#include "actor.h"

class Character : public Actor, public core::ObjectPool<Character*>
{
public:
	Character();
	virtual ~Character();

	void Initialize() override;
	void Finallize() override;

	void OnUpdate() override;

	fb::eActorType Type() const override { return fb::eActorType_Character; }
	bool IsCharacter() const  override { return true; }

	bool IsAttackable() const override { return true; }
	bool IsAttacked() const override { return true; }
	bool IsMovable() const override { return true; }
	bool IsAppear() const override { return true; }

	Coord_t X() const override;
	Coord_t Y() const override;
	Coord_t Z() const override;

	Hp_t MaxHp() const override;
	Hp_t CurHp() const override;
	Mp_t MaxMp() const override;
	Mp_t CurMp() const override;

private:
	Vector_t m_pos;
	stHpMp m_hp_mp;
};