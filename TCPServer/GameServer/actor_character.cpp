#include "pch.h"
#include "actor_character.h"
#include "server_game.h"
#include "terrain.h"
#include "terrain_grid.h"
#include "user.h"
#include "world.h"
#include "../Common/utility.h"

Character::Character()
	: m_uid(0)
{
}

Character::~Character()
{
}

void Character::Initialize()
{
	// @todo ĳ���� ���̺� ������ �ʿ��ϴ�.
}

void Character::Finalize()
{
}

bool Character::Setup(const fb::CharacterDetail& _detail)
{
	auto* base = _detail.base();
	if (nullptr == base)
	{
		return false;
	}

	m_uid = 0 < base->uid() ? base->uid() : GetWorld().m_character_uid.CreateUid();
	m_name = base->name() ? base->name()->c_str() : "";
}

bool Character::SetupDetail(const fb::CharacterDetail& _detail)
{
    return false;
}

void Character::SetActorTable(const ActorBasicTable* _table)
{
}

void Character::OnUpdate()
{
}

Result_t Character::DoMove(fb::PositionT& _position)
{
	// �̵� ���� ���� Ȯ��
	if (false == CanMove())
	{
		return fb::eResult_ActorNotMoveState;
	}

	// @todo �̵� ������ �Ÿ����� Ȯ��

	// @todo ��Ÿ �̵��� ���õ� ������ ������� �߰�

	return UpdatePosition(_position, fb::eActorMoveEffect_Normal);
}

Result_t Character::DoAttack(SkillIndex_t _index, SkillLevel_t _level, const PositionT& _cast_pos, OUT ActorList& _target_list)
{
	auto skill_table = GameServer::GetData()->FindSkillTable(_index, _level);
	if (nullptr == skill_table)
	{
		return eResult_DataNotFound;
	}

	switch(skill_table->use_type)
	{
	case eSkillUseType_Active:
	{

	}
	break;
	case eSkillUseType_Passive:
	{
		_target_list.clear();
		_target_list.insert(shared_from_this());
	}
	break;
	default:
	{
		return eResult_DataNotFound;
	}
	break;
	}

	if (true == _target_list.empty())
	{
		return eResult_SkillNoTarget;
	}

	if (skill_table->target_count < _target_list.size())
	{
		LOG_ERROR << LOG_ACTOR(this)
			<< " target count overflow."
			<< " skill index:" << _index
			<< " table count:" << skill_table->target_count
			<< " target count:" << _target_list.size();
		return eResult_SkillOverflowTarget	;
	}

	if (UTIL.CalcDistance2D(Position(), _cast_pos) > skill_table->cast_distance + 3.f) // ���� ��� ����
	{
		return eResult_SkillOverflowRange;
	}

	// ��Ʈ Ÿ�� üũ.
	eSkillHitType_Normal;
	eSkillHitType_Sequential; // ��������
	eSkillHitType_Combo; // �޺�
	eSkillHitType_Random; // ����
	eSkillHitType_Projectile; // ����ü
	skill_table->hit_type;

	std::set<std::shared_ptr<Actor>> delete_target_list;

	for (auto& target : _target_list)
	{
		if (nullptr == target)
		{
			delete_target_list.insert(target);
			continue;
		}

		// �Ÿ�����
		Distance_t distance = UTIL.CalcDistance2D(_cast_pos, target->Position());
		if (distance > skill_table->apply_distance + 3.f) // ���� ��� ����
		{
			delete_target_list.insert(target);
			continue;
		}

		switch (skill_table->target_shape)
		{
		case eSkillTargetShape_Single:
		{
		}
		break;
		case eSkillTargetShape_Line:
		{
			if (false == UTIL.CheckWithInLine(_cast_pos, target->Position()))
			{
				delete_target_list.insert(target);
				continue;
			}
		}
		break;
		case eSkillTargetShape_Cone:
		{
			if (false == UTIL.CheckWithInCone(_cast_pos, target->Position()))
			{
				delete_target_list.insert(target);
				continue;
			}
		}
		break;
		case eSkillTargetShape_Circle:
		{
			if (false == UTIL.CheckWithInCircle(_cast_pos, skill_table->apply_distance, target->Position()))
			{
				delete_target_list.insert(target);
				continue;
			}
		}
		break;
		case eSkillTargetShape_Rectangle:
		{
			if (false == UTIL.CheckWithInRectangle(_cast_pos, skill_table->apply_distance, skill_table->apply_distance, target->Position()))
			{
				delete_target_list.insert(target);
				continue;
			}
		}
		break;
		case eSkillTargetShape_Cross:
		{
			if (false == UTIL.CheckWithInCross(_cast_pos, skill_table->apply_distance, skill_table->apply_distance, target->Position()))
			{
				delete_target_list.insert(target);
				continue;
			}
		}
		break;
		case eSkillTargetShape_Dounut:
		{
			if (false == UTIL.CheckWithInDounut(_cast_pos, skill_table->apply_distance, /*@todo ����ũ��*/3.f, target->Position()))
			{
				delete_target_list.insert(target);
				continue;
			}
		}
		break;
		default:
		{
			LOG_ERROR << "Not defined targer shared. " << EnumNameeSkillTargetShape(skill_table->target_shape);
			return eResult_Fail;
		}
		break;
		}
	}

	// ���� ����� ������ ��Ͽ��� �����Ѵ�
	if (false == delete_target_list.empty())
	{
		_target_list.erase(std::remove_if(_target_list.begin(), _target_list.end(),
			[&delete_target_list](const std::shared_ptr<Actor>& actor) {
				return delete_target_list.count(actor);
			})
			, _target_list.end()
		);
	}

	for (const auto& ability : skill_table->ability_list)
	{
		// �� �ڽſ��� ����
		if (eAbilityTarget_Self == ability.target)
		{
			ApplyAbility(ability, GetActorUid(), _index, UTIL.CurrentTime() + ability.duration);
		}
		else if (ability.target == eAbilityTarget_Enermy)
		{
			for (auto& target : _target_list)
			{
				if (false == IsEnermy(target))
				{
					continue;
				}

				target->ApplyAbility(ability, GetActorUid(), _index, UTIL.CurrentTime() + ability.duration);
			}
		}
	}

	return eResult_Success;
}

Result_t Character::UpdatePosition(const fb::PositionT& _position, fb::eActorMoveEffect _move_effect)
{
	// @todo �̵� ���� ���� ���� üũ
	// @todo �̵� �ӵ� üũ
	// @todo ĳ���� ���� ������ �̵� ������ ��ǥ���� Ȯ��

	Terrain* cur_terrain = GetTerrain();
	if (nullptr == cur_terrain)
	{
		return eResult_TerrainNotIn;
	}

	TerrainGrid* cur_terrain_grid = GetTerrainGrid();

	TerrainGrid* terrain_grid = cur_terrain->FindGrid(_position);
	if (nullptr == terrain_grid)
	{
		// �� ��ǥ �ȿ� ���� ��ǥ
		return eResult_TerrainNotInThis;
	}

	do
	{
		if (cur_terrain_grid == terrain_grid)
		{
			break;
		}

		cur_terrain_grid->LeaveActor(shared_from_this(), _move_effect);
		terrain_grid->EnterActor(shared_from_this(), _position.x, _position.y, _position.z, _move_effect);

		SetTerrainGrid(terrain_grid);

	} while (false);

	if (eActorMoveEffect_Teleport == _move_effect)
	{
	}
	else
	{

	}

	float dx = _position.x - m_pos.x;
	float dy = _position.y - m_pos.y;
	
	SetPosition(_position);
	SetAngle(std::atan2(dy, dx) * 180 / g_pi);

	// @todo �ֺ��� �˸�
	// @todo ��Ƽ or ��� �˸�

	m_tick_prev_move = GetWorld().utility.CurrentTick();

	return eResult_Success;
}

Result_t Character::UpdatePosition(TerrainIdx_t _terrain_idx, const PositionT& _position, fb::eActorMoveEffect _move_effect)
{
	return eResult_Success;
}

bool Character::CanMove() const
{
	// @todo ����� ȿ���� üũ �߰�
	return true;
}

Terrain* Character::GetTerrain() const
{
	return m_terrain;
}

TerrainGrid* Character::GetTerrainGrid() const
{
	return m_grid;
}

void Character::SetTerrainGrid(TerrainGrid* _grid)
{
	m_grid = _grid;
}

const PositionT& Character::Position() const
{
	return m_pos;
}

void Character::SetPosition(const PositionT& _pos)
{
	m_pos = _pos;
}
