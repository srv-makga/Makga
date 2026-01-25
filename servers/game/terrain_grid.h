#pragma once

import makga.math.vector3;

class Terrain;
class TerrainGrid
{
	friend class Terrain;

public:
	TerrainGrid(Terrain* terrain, const makga::math::Vector3& start, Coord grid_size);
	virtual ~TerrainGrid();

	bool Initialize();
	void Finalize();

	bool AddActor(ActorId actor_id);
	bool RemoveActor(ActorId actor_id);
	bool HasActor(ActorId actor_id) const;

protected:
	Terrain* terrain_;
	makga::math::Vector3 min_;
	makga::math::Vector3 max_;
	makga::math::Vector3 center_;

	std::vector<TerrainGrid*> neighbors_;

	std::unordered_set<ActorId> actor_ids_;
};