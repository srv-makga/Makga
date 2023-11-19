#pragma once

#include "game_header.h"
#include "actor.h"

class TerrainGrid;

class Terrain
{
public:
	Terrain(TerrainUid_t _uid);

	void Initialize(const TerrainInfo& _table);
	void Finalize();

	void OnUpdate();

	bool IsInside(const Vector_t& _vector);

	TerrainIdx_t Idx() const;

private:
	TerrainUid_t m_uid;

	// 지형 타입
	// 속성(pvp 텔레포트 등)
	// actor 관련 처리
	std::vector<std::vector<TerrainGrid*>> m_grid;

	const TerrainInfo* m_table;
};