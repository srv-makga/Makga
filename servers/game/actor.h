#pragma once

#include "../common/message_actor.h"
#include "actor_board.h"
#include "detour/DetourAlloc.h"
#include "detour/DetourAssert.h"
#include "detour/DetourCommon.h"
#include "detour/DetourMath.h"
#include "detour/DetourNavMesh.h"
#include "detour/DetourNavMeshBuilder.h"
#include "detour/DetourNavMeshQuery.h"
#include "detour/DetourNode.h"
#include "detour/DetourStatus.h"

import makga.math.vector3;
import makga.lib.lock;

class Terrain;
class AIController;

class Actor : public std::enable_shared_from_this<Actor>, public MessageActor
{
	friend class AIController;

public:
	Actor();
	virtual ~Actor();

	virtual bool Initialize();
	virtual void Finalize();

	virtual makga::Result DoMove(Coord x, Coord y, Coord z, makga::MoveType movetype = makga::MoveType_Walk);

	bool SetMovePosition(float x, float y, float z);
	bool SetMovePosition(const makga::math::Vector3& _vector);

	// BT 관련 //////////////////////////////////////////////////////////////
	// @brief 주변에서 타겟 찾기
	bool IsUpdateAI() const;
	void ActiveAI();
	void DeactiveAI();


	std::shared_ptr<Actor> FindTarget();

	// @brief 타겟 변경
	bool ChangeTarget(std::shared_ptr<Actor> target);

	bool HasTarget() const;
	bool HasLeader() const;
	bool HasOwner() const;
	bool IsDead() const;
	bool IsAlive() const;
	bool IsMoveable() const;
	bool IsInSafetyArea() const;
	bool IsInAttackRange(std::shared_ptr<Actor> target) const;
	/// /////////////////////////////////////////////////////////////////////

	virtual makga::AIType GetAIType() const;
	virtual makga::ActorType GetActorType() const = 0;

public: // MessageActor overrides
	virtual bool IsValid() const { return false; }
	virtual void OnUpdate(float delta_time) override;

	virtual void PushMessage(std::unique_ptr<Message> message) = 0;
	virtual void ProcessMessages(std::unique_ptr<Message> message) = 0;

public:
	ActorId GetId() const;

	const ActorBoard& GetBoard() const;

	// @brief 현재 타겟 반환
	std::shared_ptr<Actor> GetTarget() const;
	// @brief 리더 반환
	std::shared_ptr<Actor> GetLeader() const;
	// @brief 소유자 반환
	std::shared_ptr<Actor> GetOwner() const;

	Hp GetCurHp() const;
	Hp GetMaxHp() const;
	Mp GetCurMp() const;
	Mp GetMaxMp() const;

	// 위치 관련 메서드 추가
	makga::math::Vector3 GetPosition() const;
	void SetPosition(const makga::math::Vector3& pos);

	// Terrain 접근 (TerrainGrid 대신 Terrain 참조)
	std::shared_ptr<Terrain> GetTerrain() const { return terrain_; }
	void SetTerrain(std::shared_ptr<Terrain> terrain) { terrain_ = terrain; }

protected:
	// @brief private의 내용을 public으로 복사
	void CopyPrivateToPublic();

	void SetCurHp(Hp hp);
	void SetMaxHp(Hp hp);
	void SetCurMp(Mp mp);
	void SetMaxMp(Mp mp);

	// 다른 Actor 이동 알림 처리
	virtual void OnOtherActorMove(ActorId actor_id, 
	                               const makga::math::Vector3& old_pos,
	                               const makga::math::Vector3& new_pos) {}

protected:
	inline static std::atomic<ActorId> next_id_ = 0;

	Tick last_move_tick;
	makga::math::Vector3 dest_position_;	// 이동 목적지
	std::vector<dtPolyRef> route_path_;		// 이동 경로

	// 타인에게 공유되어야할 정보
	std::unique_ptr<ActorBoard> private_board_;
	std::unique_ptr<ActorBoard> public_board_;

	std::shared_ptr<Actor> target_;
	std::shared_ptr<Actor> leader_;
	std::shared_ptr<Actor> owner_;

	bool is_update_ai_;
	std::unique_ptr<AIController> ai_controller_;

	std::shared_ptr<Terrain> terrain_;

	makga::lib::MPSCQueue<std::unique_ptr<Message>> message_queue_;
};