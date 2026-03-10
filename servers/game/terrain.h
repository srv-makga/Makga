#pragma once

#include "../common/message_actor.h"
#include "actor.h"

#include "detour/DetourNavMesh.h"
#include "detour/DetourNavMeshQuery.h"

import makga.math.vector3;
import makga.lib.mpsc_queue;

class TerrainGrid;
class Terrain : public MessageActor, public std::enable_shared_from_this<Terrain>
{
	friend class TerrainGrid;

public:
	Terrain(TerrainId terrain_id, makga::math::Vector3 min, makga::math::Vector3 max, Coord grid_size);
	virtual ~Terrain();

	bool Initialize();
	void Finalize();

	// @brief ��ġ�� �޾� �ش� �׸��� ��ȯ
	// @detail ���� ����
	TerrainGrid* GetGridAtPosition(const makga::math::Vector3& position) const;
	TerrainGrid* GetGridAtPosition(Coord x, Coord y, Coord z) const;

	// @brief n * n �� �޾� �ش� �׸��� ��ȯ
	TerrainGrid* GetGridByIndex(int x, int z) const;
	// @brief ��ġ�� �� Terrain ���� �ִ��� Ȯ��
	bool IsInPosition(const makga::math::Vector3& position) const;

	// @brief NavMesh 위에서 이동 가능한 좌표로 보정
	// @detail old_pos에서 new_pos로 이동 시 NavMesh 표면을 따라 실제 이동 가능한 좌표 반환.
	//         벽/장애물 충돌 시 막힌 지점까지만 이동. NavMesh 없으면 new_pos 그대로 반환.
	makga::math::Vector3 ClampToNavMesh(
		const makga::math::Vector3& old_pos,
		const makga::math::Vector3& new_pos) const;

	TerrainId GetTerrainId() const;
	TerrainIdx GetTerrainIdx() const;
	Coord GetGridSize() const { return grid_size_; }

public: // MessageActor
	void PushMessage(std::unique_ptr<Message> message) override;
	void ProcessMessages(std::unique_ptr<Message> message) override;
	void OnUpdate(float delta_time) override;
	bool IsValid() const override { return true; }

protected:
	// @brief Actor�� Terrain�� ��� (�ٸ� Terrain���� ����)
	bool RegisterActor(std::shared_ptr<Actor> actor);
	void OnRegistedActor(std::shared_ptr<Actor> actor);

	// @brief Actor�� Terrain���� ���� (�ٸ� Terrain���� ����)
	bool UnregisterActor(std::shared_ptr<Actor> actor);
	void OnUnregistedActor(std::shared_ptr<Actor> actor);

	// @brief Actor�� ���� Terrain ������ �׸��� �̵�
	bool MoveActor(ActorId actor_id, const makga::math::Vector3& old_position, const makga::math::Vector3& new_position);

	// Actor�� �̵��� �� �� Terrain�� �����ų� �̵��ϰų� ������ �Լ��� ����
	// Actor::Move(const makga::math::Vector3& position) �̶�� �� �� �� ��ǥ�� �ش� ������, �� Terrain������ Ȯ���ϰ�
	// ������ A Terrain, ���ο� ��ġ�� B Terrain�̶�� Register,
	// ������ B Terrain, ���ο� ��ġ�� B Terrain�̶�� UnRegister,
	// ������ A Terrain, ���ο� ��ġ�� A Terrain�̶�� Move ó��

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

	makga::lib::MPSCQueue<std::unique_ptr<Message>> message_queue_;

	dtNavMesh* nav_mesh_; // ���� ������
	static thread_local dtNavMeshQuery* nav_query_;
	dtQueryFilter* nav_filter_; // �̵� ��Ģ
};