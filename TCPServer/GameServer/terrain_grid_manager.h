#pragma once

#include <vector>

class TerrainGrid;

class TerrainGridManager
{
public:
	TerrainGridManager();
	virtual ~TerrainGridManager();

	void Initialize(const TerrainInfo& _table);
	void OnUpdate();

	bool EnterActor(std::shared_ptr<Actor> _actor, Coord_t _x, Coord_t _y, Coord_t _z);
	bool LeaveActor(std::shared_ptr<Actor> _actor);

	TerrainGrid* FindGrid(Coord_t _x, Coord_t _y, Coord_t _z);

	std::vector<std::vector<TerrainGrid*>> m_grid;
};