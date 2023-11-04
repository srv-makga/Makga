#include "pch.h"
#include "terrain_grid.h"
#include "actor.h"

TerrainGrid::TerrainGrid(CoordPoint_t& _lefttop, CoordPoint_t& _rightbottom, Coord_t _height)
	: m_lefttop(_lefttop)
	, m_rightbottom(_rightbottom)
	, m_height(_height)
{
}

TerrainGrid::~TerrainGrid()
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

bool TerrainGrid::AddActor(Actor* _actor)
{
	if (nullptr == _actor)
	{
		return false;
	}

	return m_actors.insert({ _actor->Id(), _actor }).second;
}

Actor* TerrainGrid::FindActor(Actor* _actor)
{
	if (nullptr == _actor)
	{
		return nullptr;
	}

	return FindActor(_actor->Id());
}

Actor* TerrainGrid::FindActor(ActorId_t _actor_id)
{
	auto iter = m_actors.find(_actor_id);
	if (m_actors.end() == iter)
	{
		return nullptr;
	}

	return iter->second;
}

bool TerrainGrid::Erasector(ActorId_t _actor_id)
{
	return 0 < m_actors.erase(_actor_id);
}

bool TerrainGrid::Erasector(Actor* _actor)
{
	if (nullptr == _actor)
	{
		return false;
	}

	return Erasector(_actor->Id());
}

TerrainGrid& TerrainGrid::PushAround(TerrainGrid* _terrain_grid)
{
	m_around.push_back(_terrain_grid);
	return *this;
}
