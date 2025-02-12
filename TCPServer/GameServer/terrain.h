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

	Result_t CanMove(const PositionT& _position);

	bool IsInside(const PositionT& _position);

	bool EnterActor(std::shared_ptr<Actor> _actor, Coord_t _x, Coord_t _y, Coord_t _z);
	bool LeaveActor(std::shared_ptr<Actor> _actor);

	TerrainGrid* FindGrid(const PositionT& _position);
	TerrainGrid* FindGrid(Coord_t _x, Coord_t _y, Coord_t _z);

	void FindNotificationList(const PositionT& _old,
		const PositionT& _new,
		Count_t _max_count,
		OUT ActorList& _appear_list,
		OUT ActorList& _disappear_list,
		OUT ActorList& _move_list);

	// @brief position 주변 액터 목록을 불러온다
	bool AroundList(std::shared_ptr<Actor> _actor, int _filter, OUT ActorList& _actor_list);
	// @brief position 주변 액터 목록을 불러온다
	bool AroundList(const PositionT& _position, Distance_t _range, int _filter, OUT ActorList& _actor_list);

	Count_t CurUserCount() const;


	TerrainUid_t Uid() const;
	TerrainIdx_t Idx() const;

private:
	// @brief 내비메쉬 로딩
	bool LoadNavMesh(const String8& _file_name);

private:
	TerrainUid_t m_uid;

	// 지형 타입
	// 속성(pvp 텔레포트 등)
	// actor 관련 처리

	const TerrainInfo* m_table;

	std::unordered_map<ActorUid_t, Actor*> m_actor_list;

	TerrainGridManager* m_grid_manager;

	dtNavMesh* m_nav_mesh;
	dtNavMeshQuery* m_nav_query;
	dtQueryFilter m_nav_filter;
};