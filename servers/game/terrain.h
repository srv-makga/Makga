#pragma once

#include "detour/DetourAlloc.h"
#include "detour/DetourAssert.h"
#include "detour/DetourCommon.h"
#include "detour/DetourMath.h"
#include "detour/DetourNavMesh.h"
#include "detour/DetourNavMeshBuilder.h"
#include "detour/DetourNavMeshQuery.h"
#include "detour/DetourNode.h"
#include "detour/DetourStatus.h"

import makga.math.vector3;

class Actor;
class TerrainGrid;
class Terrain
{
	friend class TerrainGrid;
public:
	Terrain(makga::math::Vector3 min, makga::math::Vector3 max, Coord grid_size);
	virtual ~Terrain();

	bool Initialize();
	void Finalize();

	// @brief 위치를 받아 해당 그리드 반환
	// @detail 높이 무시
	TerrainGrid* GetGridAtPosition(const makga::math::Vector3& position);
	TerrainGrid* GetGridAtPosition(Coord x, Coord y, Coord z);

	// @brief n * n 을 받아 해당 그리드 반환
	TerrainGrid* GetGridByIndex(int x, int z);

	TerrainId GetTerrainId() const;
	TerrainIdx GetTerrainIdx() const;

protected:
	void SetupNeighbors();
	bool LoadNavMesh(const std::string& _file_name);

protected:
	TerrainId terrain_id_;

	makga::math::Vector3 min_;
	makga::math::Vector3 max_;
	Coord grid_size_;

	int count_x_;
	int count_z_;

	std::vector<std::unique_ptr<TerrainGrid>> grids_;

	dtNavMesh* nav_mesh_;
	dtNavMeshQuery* nav_query_;
	dtQueryFilter* nav_filter_;
};