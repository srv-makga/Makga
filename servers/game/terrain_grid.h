#pragma once

class Terrain;
class TerrainGrid
{
	friend class Terrain;
public:
	TerrainGrid(Terrain* terrain);
	~TerrainGrid();

	bool Initialize();
	void Finalize();

protected:
	Terrain terrain_;
	std::vector<TerrainGrid*> neighbors_;

	TerrainIdx idx_;
};