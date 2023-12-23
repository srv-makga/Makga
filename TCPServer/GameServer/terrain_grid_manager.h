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

	std::vector<std::vector<TerrainGrid*>> m_grid;
};