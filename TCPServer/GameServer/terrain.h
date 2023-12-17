#pragma once

#include "game_header.h"
#include "actor.h"
#include "detour.h"

class TerrainGrid;

class Terrain
{
public:
	Terrain(TerrainUid_t _uid);

	void Initialize(const TerrainInfo& _table);
	void Finalize();

	void OnUpdate();

	bool IsInside(const Vector_t& _vector);

	bool AroundList(const PositionT& _position, Distance_t _range, OUT ActorList& _actor_list);


	TerrainIdx_t Idx() const;

private:
	TerrainUid_t m_uid;

	// ���� Ÿ��
	// �Ӽ�(pvp �ڷ���Ʈ ��)
	// actor ���� ó��
	std::vector<std::vector<TerrainGrid*>> m_grid;

	const TerrainInfo* m_table;
};