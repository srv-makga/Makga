#include "pch.h"
#include "action_node.h"
#include "actor_ai.h"

ActionNode::ActionNode(std::shared_ptr<ActorAI> _ai, ActionType _action)
	: Node()
	, m_ai(_ai)
	, m_action(_action)
{
}

core::ai::Node::Status ActionNode::update()
{
	if (nullptr == m_ai)
	{
		return core::ai::Node::Status::Invalid;
	}

	if (false == m_ai->IsValid())
	{
		return core::ai::Node::Status::Invalid;
	}

	return m_action(m_ai.get());
}