#pragma once
#include "game_header.h"

constexpr Coord_t GridX = 30;
constexpr Coord_t GridY = 30;

class Actor;

class TerrainGrid
{
public:
	using Actors = std::unordered_map<ActorUid_t, Actor*>;
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

	bool AddActor(Actor* _actor);
	Actor* FindActor(Actor* _actor);
	Actor* FindActor(ActorUid_t _actor_id);
	bool Erasector(ActorUid_t _actor_id);
	bool Erasector(Actor* _actor);

	bool IsInside(Coord_t _x, Coord_t _y, Coord_t _z);


	TerrainGrid& SetAround(Direction _direction, TerrainGrid* _grid);

	// @brief ��ǥ �߽����� ���� �Ÿ� ���͵� �ҷ�����
	void ActorListByCoord(Coord_t _x, Coord_t _y, Coord_t _z, Distance_t _distance, OUT ActorList& _list);
	void GridListByChangeCoord(const PositionT& _pos1, const PositionT& _pos2, OUT std::vector<TerrainGrid*>& _list);

private:
	Actors m_actors;
	CoordPoint_t m_lefttop; // �»��
	CoordPoint_t m_rightbottom; // ���ϴ�
	Coord_t m_height; // ����

	std::vector<TerrainGrid*> m_around;
};