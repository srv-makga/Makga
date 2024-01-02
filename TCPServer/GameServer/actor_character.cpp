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

	// ���� ��ġ�� �׳� ���� ó��..
	if (true == UTIL.IsSame(Position(), _position))
	{
		return eResult_Success;
	}

	// �̵� �ð� üũ
	Tick_t diff_tick = 0;

	// �̵� �Ÿ� üũ
	Distance_t distance = UTIL.CalcDistance(Position(), _position);
	// �ð� * �ӵ� < 
	if ((diff_tick / 1000) * MoveSpeed() < distance)
	{
		LOG_ERROR << LOG_ACTOR(this) << " Speed:" << MoveSpeed() << " DiffTick:" << diff_tick;
		return eResult_ActorInvalidSpeed;
	}

	// �ش� ��ǥ�� �̵������� �������� Ȯ��
	Result_t result = terrain->CanMove(_position);
	if (eResult_Success != result)
	{
		return result;
	}

	User* owner_user = OwnerUser();
	if (nullptr == owner_user)
	{
		return;
	}

	// @todo �̵��� ����ؾ� �� �͵�.. ��ȣ�ۿ� ���

	Coord_t new_x = _position.x;
	Coord_t new_y = _position.y;
	Coord_t new_z = _position.z;
	Coord_t new_angle = _position.angle;

	// @todo ��ǥ�� �����Ǿ�� �Ѵٸ� ���⼭

	ActorList appear_list;
	ActorList disappear_list;
	ActorList move_list;

	terrain->FindNotificationList(Position(),
		_position,
		SYSTEM.actor.max_move_list,
		appear_list, disappear_list, move_list);

	// Terrain �� ���� ����
	terrain->leave;

	appear_list.erase(this);
	move_list.erase(this);
	disappear_list.erase(this);

	CREATE_FBB(fbb_me_to_other);
	CREATE_FBB(fbb_other_to_me);

	// �� �ֺ��� Ž���ϸ� ������ �������ų� �����Ѵ�.

	if (false == appear_list.empty())
	{
		// �ٸ� ����鿡�� �� �� ����
		fbb_me_to_other.Clear();
		fbb_me_to_other.Finish(fb::server::CreateRecv_ActorAppear(fbb_me_to_other,
			fbb_me_to_other.CreateVector({ OffsetActorAppear(fbb_me_to_other, eActorMoveEffect::Delay) })
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

			User* user = actor->OwnerUser();
			if (nullptr != user)
			{
				user->Send(fb::server::RecvPid_ActorAppear, fbb_me_to_other);
			}

			// ������ ������ �ٸ� ���� ����
			vec_offset_appear.push_back(actor->OffsetActorAppear(fbb_other_to_me, eActorMoveEffect::Delay));
		}

		if (false == vec_offset_appear.empty())
		{
			fbb_other_to_me.Finish(fb::server::CreateRecv_ActorAppear(fbb_other_to_me,
				fbb_other_to_me.CreateVector(vec_offset_appear)));
			owner_user->Send(fb::server::RecvPid_ActorAppear, fbb_other_to_me);
		}
	}

	if (false == move_list.empty())
	{
		fbb_me_to_other.Clear();
		fbb_me_to_other.Finish(fb::server::CreateRecv_ActorMove(fbb_me_to_other,
			fbb_me_to_other.CreateVector({ OffsetActorMove(fbb_me_to_other) })
		));

		for (Actor* actor : move_list)
		{
			User* user = actor->OwnerUser();
			if (nullptr != user)
			{
				user->Send(fb::server::RecvPid_ActorMove, fbb_me_to_other);
			}
		}
	}

	if (false == disappear_list.empty())
	{
		fbb_me_to_other.Clear();
		fbb_me_to_other.Finish(fb::server::CreateRecv_ActorDisappear(fbb_me_to_other,
			fbb_me_to_other.CreateVector({ OffsetActorDisappear(fbb_me_to_other, eActorMoveEffect::Delay) })
		));

		fbb_other_to_me.Clear();

		std::vector<flatbuffers::Offset<fb::ActorDisAppear>> vec_offset_disappear;
		vec_offset_disappear.reserve(appear_list.size());

		for (Actor* actor : disappear_list)
		{
			if (nullptr == actor)
			{
				continue;
			}

			User* user = actor->OwnerUser();
			if (nullptr != user)
			{
				user->Send(fb::server::RecvPid_ActorAppear, fbb_me_to_other);
			}

			// ������ ������ �ٸ� ���� ����
			vec_offset_disappear.push_back(OffsetActorDisappear(fbb_other_to_me, eActorMoveEffect::Delay));
		}

		owner_user->Send(fb::server::RecvPid_ActorDisappear, fbb_me_to_other);
	}

	UpdatePosition(_position);

    return eResult_Success;
}

void Character::UpdatePosition(const fb::PositionT& _position)
{
	Terrain* cur_terrain = CurTerrain();
	if (nullptr == cur_terrain)
	{
		return;
	}

	cur_terrain->LeaveActor(this);
	
	SetPosition(_position);

	Terrain* new_terrain = TerrainManager->FindTerrain(_position.x, _position.y, _position.z);
	if (nullptr == new_terrain)
	{
		LOG_ERROR << "new terrain is nullptr";
		return;
	}

	new_terrain->EnterActor(this, _position.x, _position.y, _position.z);

	// @todo �̰� �ϴ� ���� ��Ƽ ���� �������� �����尡 ����ȴ�
	SetTerrain(new_terrain);
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

void Character::SetPosition(const fb::Position& _pos, Coord_t _angle)
{
	m_pos.x = _pos.x();
	m_pos.y = _pos.y();
	m_pos.z = _pos.z();
	m_pos.angle = _angle;
}

void Character::SetPosition(const Vector_t& _pos)
{
	m_pos.x = _pos.X();
	m_pos.y = _pos.Y();
	m_pos.z = _pos.Z();
}

void Character::SetPosition(Coord_t _x, Coord_t _y, Coord_t _z, Coord_t _angle)
{
	m_pos.x = _x;
	m_pos.y = _y;
	m_pos.z = _z;
	m_pos.angle = _angle;
}

Speed_t Character::Speed() const
{
	return m_speed;
}
