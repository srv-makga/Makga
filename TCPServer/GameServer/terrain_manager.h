#pragma once

#include "pch.h"
#include "terrain.h"
#include "../Common/manager_base.hpp"

class TerrainManager : public ManagerBase<TerrainUid_t, Terrain*>
{
public:
	TerrainManager() = default;
	~TerrainManager() = default;

	Terrain* NewTerrain(TerrainIdx_t _idx);
	void DeleteTerrain(TerrainUid_t _uid);

	void OnUpdate();
	void ForEach(std::function<void(Terrain*)>&& _func);

private:
	std::queue<std::pair<Time_t, Terrain*>> m_reserve_delete_list;
};