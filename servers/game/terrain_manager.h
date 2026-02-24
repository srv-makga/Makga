#pragma once

class Terrain;
class TerrainManager final
{
public:
	TerrainManager() = default;
	~TerrainManager() = default;

	bool Initialize();
	void Finalize();

	bool AddTerrain(std::shared_ptr<Terrain> terrain);
	std::shared_ptr<Terrain> GetTerrain(TerrainId terrain_id) const;
	std::optional<std::shared_ptr<Terrain>> FindTerrain(TerrainIdx terrain_idx) const;

	// @brief 월드 좌표를 포함하는 Terrain을 반환. 없으면 nullptr.
	Terrain* FindTerrainAtPosition(float x, float y, float z) const;

protected:
	std::unordered_map<TerrainId, std::shared_ptr<Terrain>> terrains_;
};