#include "pch.h"
#include "actor_ai.h"
#include "ai_aggressive.h"
#include "ai_non_aggressive.h"
#include "ai_boss.h"
#include "actor.h"

ActorAI::ActorAI(Actor* _actor, fb::eAiType _type)
	: m_actor(_actor)
	, m_node(nullptr)
{
}

ActorAI::~ActorAI()
{
	delete m_node;
}

void ActorAI::Initialize()
{
}

void ActorAI::OnUpdate()
{
	if (m_node)
	{
		m_node->tick();
	}
}

bool ActorAI::Create()
{
	switch (Type())
	{
	case eAiType_Aggressive:
		m_node = new AIAggressive(m_actor);
		break;
	case eAiType_NonAggressive:
		m_node = new AINonAggressive(m_actor);
		break;
	case eAiType_Boss:
		m_node = new AIBoss(m_actor);
		break;
	default:
		return false;
	}

	return true;
}

ActionNode::Status ActorAI::CheckSearchProcess()
{
	if (0 == m_actor->SpawnAggro())
	{
		return ActionNode::Status::Failure;
	}

	if (eAiType_NonAggressive == m_actor->AIType())
	{
		return ActionNode::Status::Failure;
	}

	return ActionNode::Status::Success;
}

ActionNode::Status ActorAI::RunSearchProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::CheckGuardProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::RunGuardProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::CheckFollowProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::RunFollowProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::CheckMoveProcess()
{
	return ActionNode::Status();
}

ActionNode::Status ActorAI::RunMoveProcess()
{
	return ActionNode::Status();
}

eAiType ActorAI::Type() const
{
	return m_actor->AIType();
}