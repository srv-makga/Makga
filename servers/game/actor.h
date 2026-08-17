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

	virtual void OnReceive(const Message& msg);

	virtual makga::Result DoMove(Coord x, Coord y, Coord z, makga::MoveType movetype = makga::MoveType_Walk);

	bool SetMovePosition(float x, float y, float z);
	bool SetMovePosition(const makga::math::Vector3& _vector);

	// GetMailbox() 제거됨 — 메시지 전달은 PushMessage() / ActorSystem::Instance().Tell() 경유

	// BT ???? //////////////////////////////////////////////////////////////
	// @brief ??????? ??? ???
	bool IsUpdateAI() const;
	void ActiveAI();
	void DeactiveAI();


	std::shared_ptr<Actor> FindTarget();

	bool HasTarget() const;
	bool HasLeader() const;
	bool HasOwner() const;
	bool IsDead() const;
	bool IsAlive() const;
	bool IsMoveable() const;
	bool IsInSafetyArea() const;
	bool IsInAttackRange(std::shared_ptr<Actor> target) const;
	bool ChangeTarget(std::shared_ptr<Actor> target);

	// @brief 이 Actor가 other를 적으로 간주하는지 여부 (서브클래스 오버라이드)
	virtual bool IsHostileTo(const Actor& other) const;
	/// /////////////////////////////////////////////////////////////////////

	virtual makga::AIType GetAIType() const;
	virtual makga::ActorType GetActorType() const = 0;

public: // MessageActor overrides
	virtual bool IsValid() const { return false; }
	virtual void OnUpdate(float delta_time) override;

	void PushMessage(std::unique_ptr<Message> message) override;
	virtual void ProcessMessages(std::unique_ptr<Message> message) override;

public:
	ActorId GetId() const;

	const ActorBoard& GetBoard() const;

	// @brief ???? ??? ???
	std::shared_ptr<Actor> GetTarget() const;
	// @brief ???? ???
	std::shared_ptr<Actor> GetLeader() const;
	// @brief ?????? ???
	std::shared_ptr<Actor> GetOwner() const;

	Hp GetCurHp() const;
	Hp GetMaxHp() const;
	Mp GetCurMp() const;
	Mp GetMaxMp() const;
	Hp GetShieldHp() const;

	// @brief 흡수 방어막 수치 설정 (SkillEffectExecutor에서 직접 호출)
	void SetShieldHp(Hp hp);

	// 전투 스탯 (서브클래스에서 Initialize 시 설정)
	virtual Attack  GetAtk() const { return 0; }
	virtual Defense GetDef() const { return 0; }

	// 죽음 처리 (서브클래스에서 오버라이드)
	virtual void OnDeath();

	// ??? ???? ????? ???
	makga::math::Vector3 GetPosition() const;
	void SetPosition(const makga::math::Vector3& pos);

	// Terrain 참조 (TerrainGrid 접근 및 Terrain 경계 이동)
	std::shared_ptr<Terrain> GetTerrain() const { return terrain_; }
	void SetTerrain(std::shared_ptr<Terrain> terrain) { terrain_ = terrain; }

	// 전투/이동 파라미터
	Distance GetAttackRange() const { return attack_range_; }
	Speed    GetMoveSpeed()   const { return move_speed_; }
	float    GetAttackInterval() const { return attack_interval_; }

	makga::math::Vector3 GetSpawnPosition() const { return spawn_position_; }
	void SetSpawnPosition(const makga::math::Vector3& pos) { spawn_position_ = pos; }

protected:
	void SetAttackRange(Distance range) { attack_range_ = range; }
	void SetMoveSpeed(Speed speed)      { move_speed_   = speed; }
	void SetAttackInterval(float sec)   { attack_interval_ = sec; }
	// @brief private?? ?????? public???? ????
	void CopyPrivateToPublic();

	void SetCurHp(Hp hp);
	void SetMaxHp(Hp hp);
	void SetCurMp(Mp mp);
	void SetMaxMp(Mp mp);

	// ??? Actor ??? ??? ???
	virtual void OnOtherActorMove(ActorId actor_id, 
	                               const makga::math::Vector3& old_pos,
	                               const makga::math::Vector3& new_pos) {}

protected:
	inline static std::atomic<ActorId> next_id_ = 0;

	Tick last_move_tick;
	makga::math::Vector3 dest_position_;    // 목적지 좌표
	makga::math::Vector3 spawn_position_;   // 스폰 위치 (귀환 판단용)
	std::vector<dtPolyRef> route_path_;     // 경로 노드

	Distance attack_range_    = 1.5f;  // 공격 사거리
	Speed    move_speed_      = 3.0f;  // 이동 속도
	float    attack_interval_ = 2.0f;  // 공격 간격 (초)

	// ──스레드 전용: private/public 이중 버퍼
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
