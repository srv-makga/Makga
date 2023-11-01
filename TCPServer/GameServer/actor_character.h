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

	void SetPos(const fb::Position* _pos);
	void SetPos(const Vector_t& _pos);
	void SetAngle(Coord_t _angle);

private:
	Vector_t m_pos;
	Coord_t m_angle;
	stHpMp m_hp_mp;
};