#pragma once

#include "../common/message_actor.h"
#include "actor_board.h"

import makga.math.vector3;
import makga.lib.lock;

class AIController;
class Actor : public std::enable_shared_from_this<Actor>, public MessageActor
{
	friend class AIController;

public:
	Actor();
	virtual ~Actor();

	bool Initialize();
	void Finalize();

	bool SetMovePosition(float x, float y, float z);
	bool SetMovePosition(const makga::math::Vector3& _vector);

	// BT 관련 //////////////////////////////////////////////////////////////
	// @brief 주변에서 타켓 찾기
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

public: // virtual
	virtual void OnUpdate(float delta_time);
	virtual makga::AIType GetAIType() const;
	virtual makga::ActorType GetActorType() const = 0;

public:
	ActorId GetId() const;

	const ActorBoard& GetBoard() const;

	// @brief 현재 타겟 반환
	std::shared_ptr<Actor> GetTarget() const;
	// @brief 리더 반환
	std::shared_ptr<Actor> GetLeader() const;
	// @brief 오너 반환
	std::shared_ptr<Actor> GetOwner() const;

	Hp GetCurHp() const;
	Hp GetMaxHp() const;
	Mp GetCurMp() const;
	Mp GetMaxMp() const;

protected:
	// @brief private의 내용을 public으로 복사
	void CopyPrivateToPublic();

	void SetCurHp(Hp hp);
	void SetMaxHp(Hp hp);
	void SetCurMp(Mp mp);
	void SetMaxMp(Mp mp);

protected:
	inline static std::atomic<ActorId> next_id_ = 0;

	ActorId id_;

	Tick last_move_tick;
	makga::math::Vector3 dest_position_;	// 이동 목적지
	std::vector<dfPolyRef> route_path_;		// 이동 경로

	// 타인에게 공유되어야할 정보
	std::unique_ptr<ActorBoard> private_board_;
	std::unique_ptr<ActorBoard> public_board_;

	std::shared_ptr<Actor> target_;
	std::shared_ptr<Actor> leader_;
	std::shared_ptr<Actor> owner_;

	bool is_update_ai_;
	std::unique_ptr<AIController> ai_controller_;
};