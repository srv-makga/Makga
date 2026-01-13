#pragma once

import makga.math.vector3;

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

	// @brief n * n 을 받아 해당 그리드 반환
	TerrainGrid* GetGridByIndex(int x, int z);

protected:
	void SetupNeighbors();

	makga::math::Vector3 min_;
	makga::math::Vector3 max_;
	Coord grid_size_;

	int count_x_;
	int count_z_;

	std::vector<std::unique_ptr<TerrainGrid>> grids_;
};