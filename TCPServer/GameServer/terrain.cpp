#include "pch.h"
#include "terrain.h"

Terrain::Terrain(TerrainUid_t _uid, const TerrainInfo& _table)
	: m_uid(_uid)
	, m_table(&_table)
{
}

void Terrain::Initialize()
{
}

bool Terrain::IsInside(const Vector_t& _vector)
{
	if (m_table->left_top.X() < _vector.X())
	{
		return false;
	}

	if (m_table->left_top.Y() < _vector.Y())
	{
		return false;
	}

	if (m_table->right_bottom.X() > _vector.X())
	{
		return false;
	}

	if (m_table->right_bottom.Y() > _vector.Y())
	{
		return false;
	}

	return true;
}

TerrainIdx_t Terrain::Idx() const
{
	return nullptr == m_table
		? 0
		: m_table->idx;
}
