#pragma once

import makga.math.vector3;
import makga.lib.lock;

class Terrain;

// @brief TerrainGrid: Terrain를 일정 크기의 격자로 나눈 단위. Actor의 위치에 따라 Grid에 속하게 되며, 인접 Grid와 함께 Actor 검색 및 메시지 브로드캐스트에 활용됨
class TerrainGrid
{
	friend class Terrain;

public:
	TerrainGrid(GridId grid_id, Terrain* terrain, const makga::math::Vector3& start, Coord grid_size);
	virtual ~TerrainGrid();

	bool Initialize();
	void Finalize();

	bool IsPositionInGrid(float x, float y, float z) const;
	bool IsPositionInGrid(const makga::math::Vector3& position) const;

	bool AddActor(ActorId actor_id);
	bool RemoveActor(ActorId actor_id);
	bool HasActor(ActorId actor_id) const;

	void BroadcastMessage(ActorId sender_id, const char* msg, size_t size) const;

	std::vector<ActorId> GetActorsCopy() const;
	
	Terrain* GetTerrain() const { return terrain_; }
	GridId GetGridId() const { return grid_id_; }
	const std::vector<TerrainGrid*>& GetNeighbors() const { return neighbors_; }

protected:
	const GridId grid_id_;

	makga::math::Vector3 min_;
	makga::math::Vector3 max_;
	makga::math::Vector3 center_;

	Terrain* terrain_;

	std::vector<TerrainGrid*> neighbors_;

	mutable makga::lib::SharedMutex mutex_;
	std::unordered_set<ActorId> actor_ids_;
};
