#pragma once
#include "game_header.h"

constexpr Coord_t GridX = 30;
constexpr Coord_t GridY = 30;

class Actor;

class TerrainGrid
{
public:
	using Actors = std::unordered_map<ActorId_t, Actor*>;

public:
	TerrainGrid(CoordPoint_t& _lefttop, CoordPoint_t& _rightbottom, Coord_t _height);
	~TerrainGrid();

	void Initialize();
	void Finalize();

	bool IsValid(Coord_t _x, Coord_t _y, Coord_t _z);

	void OnUpdate();

	bool AddActor(Actor* _actor);
	Actor* FindActor(ActorId_t _actor_id);
	bool Erasector(ActorId_t _actor_id);
	bool Erasector(Actor* _actor);


	TerrainGrid& PushAround(TerrainGrid* _terrain_grid);

private:
	Actors m_actors;
	CoordPoint_t m_lefttop; // �»��
	CoordPoint_t m_rightbottom; // ���ϴ�
	Coord_t m_height; // ����

	std::vector<TerrainGrid*> m_around;
};