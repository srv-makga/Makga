#include "pch.h"
#include "actor_character.h"
#include "data_manager.h"
#include "terrain.h"

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

Result_t Character::Move(Coord_t _x, Coord_t _y, Coord_t _z, Coord_t _angle)
{
	if (false == IsMovable())
	{
		return eResult_ActorNotMoveState;
	}

	Terrain* terrain = CurTerrain();
	if (nullptr == terrain)
	{
		return eResult_ActorNotMovePos;
	}

	// @todo 해당 좌표에 이동가능한 지역인지 확인
	// 이동할 수 없는 지역입니다.

	Coord_t old_x = X();
	Coord_t old_y = Y();
	Coord_t old_z = Z();

	Coord_t new_x = _x;
	Coord_t new_y = _y;
	Coord_t new_z = _z;
	Coord_t new_angle = _angle;

	SetPos(_x, _y, _z);
	SetAngle(_angle);

	// 내 정보 데이터

	// 내 주변을 탐색하며 정보를 가져오거나 전달한다.

	// 내 주변 유저들에게 내 정보를 전달한다

	// 내 주변 액터 목록을 가져온다
	ActorList actor_list;
	if (true == terrain->AroundList(Position(), SYSTEM.actor.max_around_distance, actor_list))
	{
	}

	// 좌표 기준 주변 알림
	GridMultiCast(this, new_x, new_y, new_z, new_angle);
	// 파티원등 알림 받아야할 대상에게 알림
	NtfyMultiCast(this, new_x, new_y, new_z, new_angle);

    return Result_t();
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

void Character::SetPos(const fb::Position* _pos)
{
	m_pos.SetX(_pos->x());
	m_pos.SetY(_pos->y());
	m_pos.SetZ(_pos->z());
	m_angle = _pos->angle();
}

void Character::SetPos(const Vector_t& _pos)
{
	m_pos = _pos;
}

void Character::SetPos(Coord_t _x, Coord_t _y, Coord_t _z)
{
	m_pos.SetX(_x);
	m_pos.SetY(_y);
	m_pos.SetZ(_z);
}

void Character::SetAngle(Coord_t _angle)
{
	m_angle = _angle;
}

Speed_t Character::Speed() const
{
	return m_speed;
}
