#pragma once

#include "ai.h"
#include <functional>
#include <memory>

class Actor;
class ActorAI;

// @breif AI 라이브러리 사용을 위한 상속 클래스
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

// @brief Actor의 함수를 조합해서 ai용 함수를 만드는 클래스
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
	// @brief 타겟 발견전 위치와 거리 체크
	ActionNode::Status CheckFarSpawnPoistion();
	ActionNode::Status MoveToTarget();

	// @brief 마지막 
	ActionNode::Status ReturnRoutePosition();

	ActionNode::Status FindTarget();
	ActionNode::Status SetTarget();

	ActionNode::Status Idle();

	fb::eAiType Type() const;

private:
	std::shared_ptr<Actor> m_actor;
	std::shared_ptr<core::ai::Node> m_node;
};