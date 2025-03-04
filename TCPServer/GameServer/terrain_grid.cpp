#include "pch.h"
#include "terrain_grid.h"
#include "actor.h"
#include "../Common/utility.h"

TerrainGrid::TerrainGrid(CoordPoint_t& _lefttop, CoordPoint_t& _rightbottom, Coord_t _height)
	: m_lefttop(_lefttop)
	, m_rightbottom(_rightbottom)
	, m_height(_height)
	, m_around(Direction::Max, nullptr)
{
}

TerrainGrid::~TerrainGrid()
{
}

void TerrainGrid::Initialize()
{
}

void TerrainGrid::Finalize()
{
}

bool TerrainGrid::IsValid(Coord_t _x, Coord_t _y, Coord_t _z)
{
	if (m_height < _z)
	{
		return false;
	}

	if (m_lefttop.first > _x || m_lefttop.second < _y)
	{
		return false;
	}

	if (m_rightbottom.first < _x || m_rightbottom.second > _y)
	{
		return false;
	}

	return true;
}

void TerrainGrid::OnUpdate()
{
	for (auto& iter: m_actors)
	{
		iter.second->OnUpdate();
	}
}

bool TerrainGrid::EnterActor(std::shared_ptr<Actor> _actor, Coord_t _x, Coord_t _y, Coord_t _z, fb::eActorMoveEffect _move_effect)
{
	return false;
}

bool TerrainGrid::LeaveActor(std::shared_ptr<Actor> _actor, fb::eActorMoveEffect _move_effect)
{
	return false;
}

bool TerrainGrid::InsertActor(std::shared_ptr<Actor> _actor)
{
	if (nullptr == _actor)
	{
		return false;
	}

	return m_actors.insert({ _actor->ActorUid(), _actor }).second;
}

std::shared_ptr<Actor> TerrainGrid::FindActor(std::shared_ptr<Actor> _actor)
{
	if (nullptr == _actor)
	{
		return nullptr;
	}

	return FindActor(_actor->ActorUid());
}

std::shared_ptr<Actor> TerrainGrid::FindActor(ActorUid_t _actor_id)
{
	auto iter = m_actors.find(_actor_id);
	if (m_actors.end() == iter)
	{
		return nullptr;
	}

	return iter->second;
}

bool TerrainGrid::Erasector(ActorUid_t _actor_id)
{
	return 0 < m_actors.erase(_actor_id);
}

bool TerrainGrid::Erasector(std::shared_ptr<Actor> _actor)
{
	if (nullptr == _actor)
	{
		return false;
	}

	return Erasector(_actor->ActorUid());
}

bool TerrainGrid::IsInside(Coord_t _x, Coord_t _y, Coord_t _z)
{
	if (m_lefttop.first > _x || m_rightbottom.first < _x)
	{
		return false;
	}

	if (m_lefttop.second > _y || m_rightbottom.second < _y)
	{
		return false;
	}

	return false;
}

TerrainGrid& TerrainGrid::SetAround(Direction _direction, TerrainGrid* _grid)
{
	m_around[_direction] = _grid;
	return *this;
}

void TerrainGrid::ActorListByCoord(Coord_t _x, Coord_t _y, Coord_t _z, Distance_t _distance, ActorSearchFilter _filter, OUT ActorList& _list)
{
	if (true == m_actors.empty())
	{
		return;
	}

	fb::PositionT pos{};
	pos.x = _x;
	pos.x = _y;
	pos.x = _z;

	for (const auto& iter : m_actors)
	{
		if (!(1 << iter.second->Type()) & _filter)
		{
			continue;
		}

		if (UTIL.CalcDistance(pos, iter.second->Position()) < _distance)
		{
			continue;
		}

		_list.insert(iter.second);
	}
}

void TerrainGrid::ActorListByPosition(const fb::PositionT& _pos, Distance_t _distance, ActorSearchFilter _filter, OUT ActorList& _list)
{
	if (true == m_actors.empty())
	{
		return;
	}

	for (const auto& iter : m_actors)
	{
		if (UTIL.CalcDistance(_pos, iter.second->Position()) < _distance)
		{
			continue;
		}

		_list.insert(iter.second);
	}
}

void TerrainGrid::GridListByChangeCoord(const PositionT& _pos1, const PositionT& _pos2, OUT std::vector<TerrainGrid*>& _list)
{
	Coord_t change_x = _pos1.x - _pos2.x;
	Coord_t change_y = _pos1.y - _pos2.y;
	Coord_t change_z = _pos1.z - _pos2.z;

	static const auto lambda_push = [](TerrainGrid* _grid, OUT std::vector<TerrainGrid*> _list)
		{
			if (nullptr != _grid)
			{
				_list.push_back(_grid);
			}
		};

	lambda_push(this, _list);

	if (0 < change_x)
	{
		if (0 < change_y)
		{
			lambda_push(m_around[Direction::Top], _list);
			lambda_push(m_around[Direction::RightTop], _list);
			lambda_push(m_around[Direction::Right], _list);
		}
		else if (0 > change_y)
		{
			lambda_push(m_around[Direction::Right], _list);
			lambda_push(m_around[Direction::Bottom], _list);
			lambda_push(m_around[Direction::RightBottom], _list);
		}
		else
		{
			lambda_push(m_around[Direction::Right], _list);
		}
	}
	else if (0 > change_x)
	{
		if (0 < change_y)
		{
			lambda_push(m_around[Direction::LeftTop], _list);
			lambda_push(m_around[Direction::Top], _list);
			lambda_push(m_around[Direction::Left], _list);
		}
		else if (0 > change_y)
		{
			lambda_push(m_around[Direction::Left], _list);
			lambda_push(m_around[Direction::LeftBottom], _list);
			lambda_push(m_around[Direction::Bottom], _list);
		}
		else
		{
			lambda_push(m_around[Direction::Left], _list);
		}
	}
	else
	{
		if (0 < change_y)
		{
			lambda_push(m_around[Direction::Top], _list);
		}
		else if (0 > change_y)
		{
			lambda_push(m_around[Direction::Bottom], _list);
		}
		else
		{
			// 좌표가 동일하다
		}
	}
}
