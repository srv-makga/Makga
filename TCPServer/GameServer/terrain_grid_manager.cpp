#include "pch.h"
#include "terrain_grid_manager.h"
#include "terrain_grid.h"
#include "actor.h"

TerrainGridManager::TerrainGridManager()
{
}

TerrainGridManager::~TerrainGridManager()
{
}

void TerrainGridManager::Initialize(const TerrainInfo& _table)
{
	Count_t grid_x_count = (_table.right_bottom.first - _table.left_top.first) / GridX + 1;
	Count_t grid_y_count = (_table.left_top.second - _table.right_bottom.second) / GridY + 1;

	m_grid.reserve(grid_x_count);

	// x
	for (Count_t i = 0; i < grid_x_count; ++i)
	{
		std::vector<TerrainGrid*> grids;
		grids.reserve(grid_y_count);

		Coord_t base_x = _table.left_top.first + (i * GridX);
		Coord_t base_y = _table.left_top.second;

		// y
		for (Count_t j = 0; j < grid_y_count; ++j)
		{
			CoordPoint_t lefttop = { base_x, base_y + (j * GridY) };
			CoordPoint_t rightbottom = { base_x + GridX, lefttop.second + GridY };

			grids.push_back(new TerrainGrid(lefttop, rightbottom, 0/* 높이 */));
		}

		m_grid.push_back(grids);
	}

	for (std::size_t i = 0; m_grid.size(); ++i)
	{
		for (std::size_t j = 0; j < m_grid[i].size(); ++j)
		{
			auto grid = m_grid[i][j];
			if (nullptr == grid)
			{
				LOG_FATAL << "grid is nullptr.";
			}

			grid->SetAround(TerrainGrid::Direction::LeftTop, (0 == i || 0 == j) ? nullptr : m_grid[i - 1][j - 1]);
			grid->SetAround(TerrainGrid::Direction::Top, (0 == i) ? nullptr : m_grid[i - 1][j]);
			grid->SetAround(TerrainGrid::Direction::RightTop, (0 == i || m_grid[i].size() == j) ? nullptr : m_grid[i - 1][j]);
			grid->SetAround(TerrainGrid::Direction::Left, (0 == j) ? nullptr : m_grid[i][j - 1]);
			grid->SetAround(TerrainGrid::Direction::Center, grid);
			grid->SetAround(TerrainGrid::Direction::Right, (m_grid[i].size() == j) ? nullptr : m_grid[i][j + 1]);
			grid->SetAround(TerrainGrid::Direction::LeftBottom, (m_grid.size() == i || 0 == j) ? nullptr : m_grid[i][j + 1]);
			grid->SetAround(TerrainGrid::Direction::Bottom, (m_grid.size() == i) ? nullptr : m_grid[i][j + 1]);
			grid->SetAround(TerrainGrid::Direction::RightBottom, (m_grid.size() == i || m_grid[i].size() == j) ? nullptr : m_grid[i + 1][j + 1]);
		}
	}
}

void TerrainGridManager::OnUpdate()
{
	for (auto& grids : m_grid)
	{
		for (TerrainGrid* grid : grids)
		{
			grid->OnUpdate();
		}

	}
}

// 액터의 terrain 입장 순서
// Actor->DoMove();
// Actor->UpdatePosition();
// TerrainManager->FindTerrain();
// Terrain->FindGrid();
// TerrainGrid->InsertActor();

TerrainGrid* TerrainGridManager::FindGrid(Coord_t _x, Coord_t _y, Coord_t _z)
{
	for (auto grids : m_grid)
	{
		for (TerrainGrid* grid : grids)
		{
			if (true == grid->IsInside(_x, _y, _z))
			{
				return grid;
			}
		}
	}

	return nullptr;
}
