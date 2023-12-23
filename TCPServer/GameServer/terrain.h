#pragma once

// fbx -> obj 파일 변환
// 

#include "game_header.h"
#include "actor.h"
#include "detour.h"

class TerrainGrid;
class TerrainGridManager;

class Terrain
{
public:
	Terrain(TerrainUid_t _uid);
	virtual ~Terrain();

	bool Initialize(const TerrainInfo& _table);
	void Finalize();

	void OnUpdate();

	Result_t DoMove(Actor* _actor, const PositionT& _position);

	bool IsInside(const Vector_t& _vector);

	bool AroundList(const PositionT& _position, Distance_t _range, OUT ActorList& _actor_list);


	TerrainIdx_t Idx() const;

private:
	bool LoadNavMesh(const String8& _file_name);

private:
	TerrainUid_t m_uid;

	// 지형 타입
	// 속성(pvp 텔레포트 등)
	// actor 관련 처리

	const TerrainInfo* m_table;

	TerrainGridManager* m_grid_manager;

	dtNavMesh* m_nav_mesh;
	dtNavMeshQuery* m_nav_query;
	dtQueryFilter* m_nav_filter;
};