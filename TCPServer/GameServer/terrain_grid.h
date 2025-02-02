#pragma once
#include "game_header.h"

constexpr Coord_t GridX = 30;
constexpr Coord_t GridY = 30;

class Actor;

class TerrainGrid
{
public:
	using Actors = std::unordered_map<ActorUid_t, std::shared_ptr<Actor>>;
	enum Direction
	{
		LeftTop = 0,
		Top,
		RightTop,
		Left,
		Center,
		Right,
		LeftBottom,
		Bottom,
		RightBottom,
		Max
	};

public:
	TerrainGrid(CoordPoint_t& _lefttop, CoordPoint_t& _rightbottom, Coord_t _height);
	~TerrainGrid();

	void Initialize();
	void Finalize();

	bool IsValid(Coord_t _x, Coord_t _y, Coord_t _z);

	void OnUpdate();

	bool EnterActor(std::shared_ptr<Actor> _actor, Coord_t _x, Coord_t _y, Coord_t _z, fb::eActorMoveEffect _move_effect);
	bool LeaveActor(std::shared_ptr<Actor> _actor, fb::eActorMoveEffect _move_effect);
	std::shared_ptr<Actor> FindActor(std::shared_ptr<Actor> _actor);
	std::shared_ptr<Actor> FindActor(ActorUid_t _actor_id);

	bool IsInside(Coord_t _x, Coord_t _y, Coord_t _z);


	TerrainGrid& SetAround(Direction _direction, TerrainGrid* _grid);

	// @brief 좌표 중심으로 일정 거리 액터들 불러오기
	void ActorListByCoord(Coord_t _x, Coord_t _y, Coord_t _z, Distance_t _distance, ActorSearchFilter _filter, OUT ActorList& _list);
	void ActorListByPosition(const fb::PositionT& _pos, Distance_t _distance, ActorSearchFilter _filter, OUT ActorList& _list);
	void GridListByChangeCoord(const PositionT& _pos1, const PositionT& _pos2, OUT std::vector<TerrainGrid*>& _list);

private:
	bool InsertActor(std::shared_ptr<Actor> _actor);
	bool Erasector(ActorUid_t _actor_id);
	bool Erasector(std::shared_ptr<Actor> _actor);

private:
	Actors m_actors;
	CoordPoint_t m_lefttop; // 좌상단
	CoordPoint_t m_rightbottom; // 우하단
	Coord_t m_height; // 높이

	std::vector<TerrainGrid*> m_around;
};