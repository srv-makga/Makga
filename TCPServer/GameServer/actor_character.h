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
	Result_t Move(Coord_t _x, Coord_t _y, Coord_t _z, Coord_t _angle) override;

public:
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
	void SetPos(Coord_t _x, Coord_t _y, Coord_t _z);
	void SetAngle(Coord_t _angle);
	
	Speed_t Speed() const override;

private:
	Vector_t m_pos;
	Coord_t m_angle;
	fb::HpMpT m_hp_mp;

	Speed_t m_speed;
};