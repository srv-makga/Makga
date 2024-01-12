#pragma once

#include "actor.h"

class Character : public Actor, private core::ObjectPool<Character*>
{
public:
	Character();
	virtual ~Character();

	void Initialize() override;
	void Finallize() override;

	void OnUpdate() override;
	Result_t DoMove(const PositionT& _position) override;

	void UpdatePosition(const fb::PositionT& _position);

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

	void SetPosition(const fb::Position& _pos, Coord_t _angle);
	void SetPosition(const Vector_t& _pos);
	void SetPosition(Coord_t _x, Coord_t _y, Coord_t _z, Coord_t _angle = 0);
	
	Speed_t Speed() const override;

public:
	static Character* Rental()
	{
		return Pop();
	}
	static void Return(Character* _actor)
	{
		Push(_actor);
	}

private:
	std::string m_name;
	fb::PositionT m_pos;
	fb::HpMpT m_hp_mp;

	Speed_t m_speed;
};