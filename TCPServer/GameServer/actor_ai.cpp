#include "pch.h"
#include "actor_ai.h"
#include "actor.h"

ActorAI::ActorAI(Actor* _actor)
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
	ChangeType(m_actor->AIType());
}

void ActorAI::OnUpdate()
{
	if (m_node)
	{
		m_node->tick();
	}
}

eAiType ActorAI::Type() const
{
	return eAiType();
}

void ActorAI::ChangeType(eAiType _type)
{
	if (Type() == _type)
	{
		return;
	}

	delete m_node;

	switch (_type)
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
	}
}