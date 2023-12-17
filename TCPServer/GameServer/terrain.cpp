#include "pch.h"
#include "terrain.h"
#include "terrain_grid.h"

Terrain::Terrain(TerrainUid_t _uid)
	: m_uid(_uid)
{
}

void Terrain::Initialize(const TerrainInfo& _table)
{
	m_table = &_table;

	Count_t grid_x_count = (m_table->right_bottom.first - m_table->left_top.first) / GridX + 1;
	Count_t grid_y_count = (m_table->left_top.second - m_table->right_bottom.second) / GridY + 1;

	m_grid.reserve(grid_x_count);

	// x
	for (Count_t i = 0; i < grid_x_count; ++i)
	{
		std::vector<TerrainGrid*> grids;
		grids.reserve(grid_y_count);

		Coord_t base_x = m_table->left_top.first + (i * GridX);
		Coord_t base_y = m_table->left_top.second;

		// y
		for (Count_t j = 0; j < grid_y_count; ++j)
		{
			CoordPoint_t lefttop = { base_x, base_y + (j * GridY) };
			CoordPoint_t rightbottom = { base_x + GridX, lefttop.second + GridY };

			grids.push_back(new TerrainGrid(lefttop, rightbottom, 0/* ���� */));
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

			if (0 < i)
				grid->PushAround(m_grid[i-1][j]);
			if (m_grid.size() > i + 1)
				grid->PushAround(m_grid[i+1][j]);
			if (0 < j)
				grid->PushAround(m_grid[i][j-1]);
			if (0 < m_grid[i].size())
				grid->PushAround(m_grid[i][j + 1]);
		}
	}
}

void Terrain::Finalize()
{
	m_grid.clear();
}

void Terrain::OnUpdate()
{
	for (auto& x : m_grid)
	{
		for (auto& y : x)
		{
			y->OnUpdate();
		}
	}
}

bool Terrain::IsInside(const Vector_t& _vector)
{
	if (m_table->left_top.first < _vector.X())
	{
		return false;
	}

	if (m_table->left_top.second < _vector.Y())
	{
		return false;
	}

	if (m_table->right_bottom.first > _vector.X())
	{
		return false;
	}

	if (m_table->right_bottom.second > _vector.Y())
	{
		return false;
	}

	return true;
}

bool Terrain::AroundList(const PositionT& _position, Distance_t _range, OUT ActorList& _actor_list)
{
	// @todo ����
	return !_actor_list.empty();
}

TerrainIdx_t Terrain::Idx() const
{
	return nullptr == m_table
		? 0
		: m_table->idx;
}