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

	// @brief private의 내용을 public으로 복사
	void CopyPrivateToPublic();

	// BT 관련 //////////////////////////////////////////////////////////////
	// @brief 주변에서 타켓 찾기
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

public: // get/set
	ActorId GetId() const;
	void SetId(ActorId id);

	std::shared_ptr<Stat> GetPublicStat() const;

	// @brief 현재 타겟 반환
	std::shared_ptr<Actor> GetTarget() const;
	// @brief 리더 반환
	std::shared_ptr<Actor> GetLeader() const;
	// @brief 오너 반환
	std::shared_ptr<Actor> GetOwner() const;

	Hp GetCurrentHp() const;
	void SetCurrentHp(Hp hp);

	Hp GetMaxHp() const;
	void SetMaxHp(Hp hp);

	Mp GetCurrentMp() const;
	void SetCurrentMp(Mp mp);

	Mp GetMaxMp() const;
	void SetMaxMp(Mp mp);

protected:
	ActorId id_;

	std::unique_ptr<AIController> ai_controller_;

	std::unique_ptr<ActorBoard> private_board_;
	std::shared_ptr<ActorBoard> public_board_;

	std::shared_ptr<Actor> target_;
	std::shared_ptr<Actor> leader_;
	std::shared_ptr<Actor> owner_;

	Hp current_hp_;
	Hp max_hp_;

	Mp current_mp_;
	Mp max_mp_;

	Tick last_move_tick;
};