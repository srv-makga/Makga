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

	ActionNode::Status HasDefaultAggro();

	ActionNode::Status IsMoveable();
	ActionNode::Status HasTarget();
	ActionNode::Status FindTarget();

	ActionNode::Status MoveNextPos();

	eAiType Type() const;

private:
	Actor* m_actor;
	core::ai::Node* m_node;
};