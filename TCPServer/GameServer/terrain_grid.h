#pragma once
#include "game_header.h"

class Actor;

class TerrainGrid
{
public:
	using Actors = std::unordered_map<ActorId_t, Actor*>;
	using Point = std::pair<Coord_t, Coord_t>;

public:
	TerrainGrid(Point& _lefttop, Point& _rightbottom, Coord_t _height);
	~TerrainGrid();

	void Initialize();
	void Finalize();

	bool IsValid(Coord_t _x, Coord_t _y, Coord_t _z);

	void OnUpdate();

	bool AddActor(Actor* _actor);
	Actor* FindActor(ActorId_t _actor_id);
	bool Erasector(ActorId_t _actor_id);
	bool Erasector(Actor* _actor);

private:
	Actors m_actors;
	Point m_lefttop; // 좌상단
	Point m_rightbottom; // 우하단
	Coord_t m_height; // 높이
};