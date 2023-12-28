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
	using ActorList = std::unordered_map<ActorUid_t, Actor*>;

public:
	Terrain(TerrainUid_t _uid);
	virtual ~Terrain();

	bool Initialize(const TerrainInfo& _table);
	void Finalize();

	void OnUpdate();

	Result_t CanMove(const PositionT& _position);

	bool IsInside(const Vector_t& _vector);

	TerrainGrid* FindGrid(Coord_t _x, Coord_t _y, Coord_t _z);

	void FindNotificationList(const PositionT& _old,
		const PositionT& _new,
		Count_t _max_count,
		OUT UserList& _appear_list,
		OUT UserList& _disappear_list,
		OUT UserList& _move_list);

	bool AroundList(const PositionT& _position, Distance_t _range, OUT ActorList& _actor_list);

	Count_t CurUserCount() const;


	TerrainIdx_t Idx() const;

private:
	bool LoadNavMesh(const String8& _file_name);

private:
	TerrainUid_t m_uid;

	// 지형 타입
	// 속성(pvp 텔레포트 등)
	// actor 관련 처리

	const TerrainInfo* m_table;

	ActorList m_actor_list;

	TerrainGridManager* m_grid_manager;

	dtNavMesh* m_nav_mesh;
	dtNavMeshQuery* m_nav_query;
	dtQueryFilter m_nav_filter;
};