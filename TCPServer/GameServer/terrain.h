#pragma once

#include "game_header.h"
#include "actor.h"

class Terrain
{
	TerrainUid_t m_uid;

	// ���� Ÿ��
	// �Ӽ�(pvp �ڷ���Ʈ ��)
	// actor ���� ó��

	const TerrainInfo* m_table;

public:
	Terrain(TerrainUid_t _uid, const TerrainInfo& _table);

	void Initialize();
	void OnUpdate();

	bool IsInside(const Vector_t& _vector);

public:
	TerrainIdx_t Idx() const;
};
