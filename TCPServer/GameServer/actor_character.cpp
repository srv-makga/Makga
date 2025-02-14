#include "pch.h"
#include "actor_character.h"
#include "data_manager.h"
#include "terrain.h"
#include "terrain_grid.h"
#include "user.h"
#include "world.h"
#include "../Common/utility.h"

Character::Character()
	: m_uid(0)
	, m_table(nullptr)
{
}

Character::~Character()
{
}

void Character::Initialize()
{
}

void Character::Finalize()
{
}

void Character::SetActorTable(const ActorBasicTable* _table)
{
	m_table = static_cast<const MonsterBasicTable*>(_table);
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
