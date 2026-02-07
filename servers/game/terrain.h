#pragma once

#include "actor.h"

import makga.math.vector3;

class Actor;
class TerrainGrid;
class Terrain
{
	friend class TerrainGrid;
public:
	using ActorList = std::unordered_map<ActorId, std::weak_ptr<Actor>>;

	Terrain(makga::math::Vector3 min, makga::math::Vector3 max, Coord grid_size);
	virtual ~Terrain();

	bool Initialize();
	void Finalize();

	

	// @brief 위치가 이 Terrain 내에 있는지 확인
	bool IsPositionInTerrain(const makga::math::Vector3& position) const;

	// @brief Actor를 Terrain에 등록 (다른 Terrain에서 들어옴)
	bool RegisterActor(ActorId actor_id, std::shared_ptr<Actor> actor, const makga::math::Vector3& position);
	void OnRegistedActor(std::shared_ptr<Actor> actor);

	// @brief Actor를 Terrain에서 제거 (다른 Terrain으로 나감)
	bool UnregisterActor(ActorId actor_id);
	void OnUnregistedActor(std::weak_ptr<Actor> actor);

	// @brief Actor가 같은 Terrain 내에서 그리드 이동
	bool MoveActor(ActorId actor_id, const makga::math::Vector3& old_position, const makga::math::Vector3& new_position);

	// Actor가 이동할 때 이 Terrain에 들어오거나 이동하거나 나가는 함수들 구현
	// Actor::Move(const makga::math::Vector3& position) 이라고 할 때 그 좌표가 해당 맵인지, 이 Terrain인지를 확인하고
	// 기존이 A Terrain, 새로운 위치가 B Terrain이라면 Register,
	// 기존이 B Terrain, 새로운 위치가 B Terrain이라면 UnRegister,
	// 기존이 A Terrain, 새로운 위치가 A Terrain이라면 Move 처리

	// @brief 위치를 받아 해당 그리드 반환
	// @detail 높이 무시
	TerrainGrid* GetGridAtPosition(const makga::math::Vector3& position) const;
	TerrainGrid* GetGridAtPosition(Coord x, Coord y, Coord z) const;

	// @brief n * n 을 받아 해당 그리드 반환
	TerrainGrid* GetGridByIndex(int x, int z);

	TerrainId GetTerrainId() const;
	TerrainIdx GetTerrainIdx() const;

protected:
	void SetupNeighbors();
	bool LoadNavMesh(const std::string& _file_name);

protected:
	TerrainId terrain_id_;

	makga::math::Vector3 min_;
	makga::math::Vector3 max_;
	Coord grid_size_;

	int count_x_;
	int count_z_;

	std::vector<std::unique_ptr<TerrainGrid>> grids_;

	dtNavMesh* nav_mesh_; // 지형 데이터
	static thread_local dtNavMeshQuery* nav_query_;
	dtQueryFilter* nav_filter_; // 이동 규칙

	ActorList actor_list_;
};