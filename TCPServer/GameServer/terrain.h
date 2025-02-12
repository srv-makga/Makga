#pragma once

// fbx -> obj ���� ��ȯ
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

	// @brief position �ֺ� ���� ����� �ҷ��´�
	bool AroundList(std::shared_ptr<Actor> _actor, int _filter, OUT ActorList& _actor_list);
	// @brief position �ֺ� ���� ����� �ҷ��´�
	bool AroundList(const PositionT& _position, Distance_t _range, int _filter, OUT ActorList& _actor_list);

	Count_t CurUserCount() const;


	TerrainUid_t Uid() const;
	TerrainIdx_t Idx() const;

private:
	// @brief ����޽� �ε�
	bool LoadNavMesh(const String8& _file_name);

private:
	TerrainUid_t m_uid;

	// ���� Ÿ��
	// �Ӽ�(pvp �ڷ���Ʈ ��)
	// actor ���� ó��

	const TerrainInfo* m_table;

	std::unordered_map<ActorUid_t, Actor*> m_actor_list;

	TerrainGridManager* m_grid_manager;

	dtNavMesh* m_nav_mesh;
	dtNavMeshQuery* m_nav_query;
	dtQueryFilter m_nav_filter;
};