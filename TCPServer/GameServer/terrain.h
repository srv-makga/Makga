#pragma once

#include "game_header.h"
#include "actor.h"

class Terrain
{
	TerrainUid_t m_uid;

	// 지형 타입
	// 속성(pvp 텔레포트 등)
	// actor 관련 처리

	const TerrainInfo* m_table;

public:
	Terrain(TerrainUid_t _uid, const TerrainInfo& _table);

	void Initialize();
	void OnUpdate();

	bool IsInside(const Vector_t& _vector);

public:
	TerrainIdx_t Idx() const;
};
