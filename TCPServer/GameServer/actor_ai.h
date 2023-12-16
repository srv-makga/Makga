#pragma once

#include "ai_base.h"

class Actor;
class ActorAI;

class ActionNode : public core::ai::Node
{
	using ActionType = Status(ActorAI::*)(void);
public:
	ActionNode(ActorAI* _ai, ActionType _action)
		: m_ai(_ai)
		, m_action(_action)
	{}

	Status update() override
	{
		return (m_ai->*m_action)();
	}

private:
	ActorAI* m_ai;
	ActionType m_action;
};

class ActorAI
{
public:
	ActorAI(Actor* _actor, fb::eAiType _type);
	virtual ~ActorAI();

	void Initialize();
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

	eAiType Type() const;

private:
	Actor* m_actor;
	core::ai::Node* m_node;
};