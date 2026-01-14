#pragma once

import makga.math.vector3;

class AIController;
class Actor
{
	friend class AIController;

public:
	Actor();
	virtual ~Actor();

	bool Initialize();
	void Finalize();

	bool SetMovePosition(float x, float y, float z);
	bool SetMovePosition(const makga::math::Vector3& _vector);

	// @brief 주변에서 타켓 찾기
	std::shared_ptr<Actor> FindTarget();
	// @brief 타겟 변경
	bool ChangeTarget(std::shared_ptr<Actor> target);

	bool IsDead() const;
	bool HasTarget() const;

	makga::AIType GetAIType() const;

public: // virtual
	virtual makga::ActorType GetActorType() const = 0;

public: // get/set
	ActorId GetId() const;
	void SetId(ActorId id);

protected:
	ActorId id_;
	std::shared_ptr<Actor> target_;

	Hp current_hp_;
	Hp max_hp_;

	Mp current_mp_;
	Mp max_mp_;
};