#pragma once

#include "actor.h"

class Pet : public Actor, private core::ObjectPool<Pet*>
{
public:
	Pet(ActorUid_t _uid);
	virtual ~Pet();

	void Initialize() override;
	void Finalize() override;

	void OnUpdate() override;
	Result_t DoMove(const PositionT& _position) override;

public:
	fb::eActorType Type() const override { return fb::eActorType_Pran; }
	bool IsCharacter() const  override { return true; }

	bool IsAttackable() const override { return true; }
	bool IsAttacked() const override { return true; }
	bool IsMovable() const override { return true; }
	bool IsSendAppear() const override { return true; }

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

public:
	static Pet* Rental()
	{
		return Pop();
	}
	static void Return(Pet* _actor)
	{
		Push(_actor);
	}

private:
	std::string m_name;
	fb::PositionT m_pos;
	fb::HpMpT m_hp_mp;

	Speed_t m_speed;
};