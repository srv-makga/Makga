#include "pch.h"
#include "actor_character.h"

Character::Character()
{
}

Character::~Character()
{
}

void Character::Initialize()
{
}

void Character::Finallize()
{
}

void Character::OnUpdate()
{
}

Coord_t Character::X() const
{
	return m_pos.X();
}

Coord_t Character::Y() const
{
	return m_pos.Y();
}

Coord_t Character::Z() const
{
	return m_pos.Z();
}

Hp_t Character::MaxHp() const
{
	return m_hp_mp.max_hp;
}

Hp_t Character::CurHp() const
{
	return m_hp_mp.cur_hp;
}

Mp_t Character::MaxMp() const
{
	return m_hp_mp.max_mp;
}

Mp_t Character::CurMp() const
{
	return m_hp_mp.cur_mp;
}
