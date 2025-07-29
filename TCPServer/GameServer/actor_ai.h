#pragma once

#include "ai.h"
#include <functional>
#include <memory>

class Actor;
class ActorAI;

// @breif AI ���̺귯�� ����� ���� ��� Ŭ����
class ActionNode : public core::ai::Node, public std::enable_shared_from_this<ActionNode>
{
	using ActionType = std::function<core::ai::Node::Status(ActorAI*)>;

public:
	ActionNode(std::shared_ptr<ActorAI> _ai, ActionType _action);
	virtual ~ActionNode() = default;	

	core::ai::Node::Status update() override;

private:
	std::shared_ptr<ActorAI> m_ai;
	ActionType m_action;
};

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