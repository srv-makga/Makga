#include "pch.h"
#include "actor_character.h"
#include "data_manager.h"
#include "terrain.h"
#include "user.h"
#include "../Common/utility.h"

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

Result_t Character::DoMove(const PositionT& _position)
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

	// 같은 위치면 그냥 성공 처리..
	if (true == UTIL.IsSame(Position(), _position))
	{
		return eResult_Success;
	}

	// 이동 시간 체크
	Tick_t diff_tick = 0;

	// 이동 거리 체크
	Distance_t distance = UTIL.CalcDistance(Position(), _position);
	// 시간 * 속도 < 
	if ((diff_tick / 1000) * MoveSpeed() < distance)
	{
		LOG_ERROR << LOG_ACTOR(this) << " Speed:" << MoveSpeed() << " DiffTick:" << diff_tick;
		return eResult_ActorInvalidSpeed;
	}

	// 해당 좌표가 이동가능한 지역인지 확인
	Result_t result = terrain->CanMove(_position);
	if (eResult_Success != result)
	{
		return result;
	}

	User* owner_user = OwnerUser();
	if (nullptr == owner_user)
	{

	}

	// @todo 이동시 취소해야 될 것들.. 상호작용 등등

	Coord_t new_x = _position.x;
	Coord_t new_y = _position.y;
	Coord_t new_z = _position.z;
	Coord_t new_angle = _position.angle;

	// @todo 좌표가 보정되어야 한다면 여기서

	ActorList appear_list;
	ActorList disappear_list;
	ActorList move_list;

	terrain->FindNotificationList(Position(),
		_position,
		SYSTEM.actor.max_move_list,
		appear_list, disappear_list, move_list);

	SetPos(new_x, new_y, new_z);
	SetAngle(new_angle);

	// Terrain 내 정보 갱신
	terrain->leave;

	appear_list.erase(this);
	move_list.erase(this);
	disappear_list.erase(this);

	CREATE_FBB(fbb_me_to_other);
	CREATE_FBB(fbb_other_to_me);

	// 내 주변을 탐색하며 정보를 가져오거나 전달한다.

	if (false == appear_list.empty())
	{
		// 다른 사람들에게 줄 내 정보
		fbb_me_to_other.Clear();

		std::vector offset_appear = { fb::CreateActorAppear(fbb_me_to_other,
			Uid(),
			OffsetActorInfoBase(fbb_me_to_other), eActorMoveEffect::Delay)
		};

		fbb_me_to_other.Finish(fb::server::CreateRecv_ActorAppear(fbb_me_to_other,
			fbb_me_to_other.CreateVector(offset_appear)
		));

		fbb_other_to_me.Clear();

		std::vector<flatbuffers::Offset<fb::ActorAppear>> vec_offset_appear;
		vec_offset_appear.reserve(appear_list.size());

		for (Actor* actor : appear_list)
		{
			if (nullptr == actor)
			{
				continue;
			}

			// 나에게 보내줄 다른 액터 정보
			vec_offset_appear.push_back(fb::CreateActorAppear(fbb_other_to_me,
				actor->Uid(),
				actor->OffsetActorInfoBase(fbb_other_to_me),
				eActorMoveEffect::Delay
			));
	
			User* user = actor->OwnerUser();
			if (nullptr != user)
			{
				user->Send(fb::server::RecvPid_ActorAppear, fbb_me_to_other);
			}
		}

		if (false == vec_offset_appear.empty())
		{
			fbb_other_to_me.Finish(fb::server::CreateRecv_ActorAppear(fbb_other_to_me,
				fbb_other_to_me.CreateVector(vec_offset_appear)));
			Send(fb::server::RecvPid_ActorAppear, fbb_other_to_me);
		}
	}

	if (false == move_list.empty())
	{
		fbb_me_to_other.Clear();
		fbb_me_to_other.Finish(fb::server::CreateRecv_ActorMove(fbb_me_to_other,
			Uid(),
			Speed()
		));

		for (Actor* actor : move_list)
		{
			if (nullptr != actor && nullptr != actor->OwnerUser())
			{
				actor->OwnerUser()->Send(fb::server::RecvPid_ActorMove, fbb_me_to_other);
			}
		}
	}

	if (false == disappear_list.empty())
	{
		fbb_me_to_other.Clear();
		fbb_me_to_other.Finish(fb::server::CreateRecv_ActorDisappear(fbb_me_to_other,
			Uid(),
			fb::eMoveType_Move
		));

		fbb_other_to_me.Clear();

		std::vector<flatbuffers::Offset<fb::ActorAppear>> vec_offset_disappear;
		vec_offset_disappear.reserve(appear_list.size());

		for (Actor* actor : disappear_list)
		{
			if (nullptr == actor)
			{
				continue;
			}

			// 나에게 보내줄 다른 액터 정보
			vec_offset_disappear.push_back(fb::Create_ActorDisappear(fbb_other_to_me, );
	
			User* user = actor->OwnerUser();
			if (nullptr != user)
			{
				user->Send(fb::server::RecvPid_ActorAppear, fbb_me_to_other);
			}

			if (nullptr != actor && nullptr != actor->OwnerUser())
			{
				actor->OwnerUser()->Send(fb::server::RecvPid_ActorDisappear, fbb_me_to_other);
			}
		}
	}

	// 파티원등 알림 받아야할 대상에게 알림
	NtfyMultiCast(this, CurTerrainIdx(), new_x, new_y, new_z);

    return eResult_Success;
}

Coord_t Character::X() const
{
	return m_pos.x;
}

Coord_t Character::Y() const
{
	return m_pos.y;
}

Coord_t Character::Z() const
{
	return m_pos.z;
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

void Character::SetGrid(const fb::Position& _pos)
{
	Terrain* terrain = CurTerrain();

	terrain->
	
	terrain->actor
}

void Character::SetPos(const fb::Position& _pos)
{
	m_pos.x = _pos.x();
	m_pos.y = _pos.y();
	m_pos.z = _pos.z();
}

void Character::SetPos(const Vector_t& _pos)
{
	m_pos.x = _pos.X();
	m_pos.y = _pos.Y();
	m_pos.z = _pos.Z();
}

void Character::SetPos(Coord_t _x, Coord_t _y, Coord_t _z)
{
	m_pos.x = _x;
	m_pos.y = _y;
	m_pos.z = _z;
}

void Character::SetAngle(Coord_t _angle)
{
	m_pos.angle = _angle;
}

Speed_t Character::Speed() const
{
	return m_speed;
}
