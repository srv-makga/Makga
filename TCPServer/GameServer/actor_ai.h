#pragma once

#include "ai.h"
#include "action_node.h"

class Actor;

// @brief Actor�� �Լ��� �����ؼ� ai�� �Լ��� ����� Ŭ����
class ActorAI
{
public:
	ActorAI(std::shared_ptr<Actor> _actor, fb::eAiType _type);
	virtual ~ActorAI();

	void Initialize();
	void Finalize();
	bool IsValid() const;

	void OnUpdate();

	bool Create();

	ActionNode::Status IsDead();
	ActionNode::Status TryResurrection();

	ActionNode::Status HasTarget();
	ActionNode::Status IsInsideAttackRange();
	ActionNode::Status AttackTarget();
	// @brief Ÿ�� �߰��� ��ġ�� �Ÿ� üũ
	ActionNode::Status CheckFarSpawnPoistion();
	ActionNode::Status MoveToTarget();

	// @brief ������
	ActionNode::Status ReturnRoutePosition();

	ActionNode::Status FindTarget();
	ActionNode::Status SetTarget();

	ActionNode::Status Idle();

	fb::eAiType Type() const;

private:
	std::shared_ptr<Actor> m_actor;
	std::shared_ptr<core::ai::Node> m_node;
};