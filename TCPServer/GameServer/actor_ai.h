#pragma once

#include "ai_base.h"
#include <functional>
#include <memory>

class Actor;
class ActorAI;

class ActionNode : public core::ai::Node
{
	using ActionType = std::function<core::ai::Node::Status(ActorAI*)>;
public:
	ActionNode(std::shared_ptr<ActorAI> _ai, ActionType _action)
		: m_ai(_ai)
		, m_action(_action)
	{}

	core::ai::Node::Status update() override
	{
		return m_action(m_ai.get());
		//return m_action(m_ai);
	}

private:
	std::shared_ptr<ActorAI> m_ai;
	ActionType m_action;
};

class ActorAI
{
public:
	ActorAI(std::shared_ptr<Actor> _actor, fb::eAiType _type);
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

	fb::eAiType Type() const;

private:
	std::shared_ptr<Actor> m_actor;
	std::atomic<core::ai::Node*> m_node;
};