#pragma once

import makga.math.vector3;
import makga.lib.lock;

class Stat;
class AIController;
class Actor : public std::enable_shared_from_this<Actor>
{
	friend class AIController;

public:
	Actor();
	virtual ~Actor();

	bool Initialize();
	void Finalize();

	bool SetMovePosition(float x, float y, float z);
	bool SetMovePosition(const makga::math::Vector3& _vector);

	// @brief private_stat_의 내용을 public_stat_으로 복사
	void CopyPrivateStatToPublicStat();

	// BT 관련 //////////////////////////////////////////////////////////////
	// @brief 주변에서 타켓 찾기
	std::shared_ptr<Actor> FindTarget();
	// @brief 현재 타겟 반환
	std::shared_ptr<Actor> GetTarget() const;
	// @brief 리더 반환
	std::shared_ptr<Actor> GetLeader() const;
	// @brief 오너 반환
	std::shared_ptr<Actor> GetOwner() const;

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

protected:
	ActorId id_;

	std::unique_ptr<AIController> ai_controller_;

	std::unique_ptr<Stat> private_stat_; // 내부 계산용

	makga::lib::SharedMutex stat_mutex_;
	std::shared_ptr<Stat> public_stat_; // 내부 계산 후 공개용

	std::shared_ptr<Actor> target_;
	std::shared_ptr<Actor> leader_;
	std::shared_ptr<Actor> owner_;

	Hp current_hp_;
	Hp max_hp_;

	Mp current_mp_;
	Mp max_mp_;

	Tick last_move_tick;
};