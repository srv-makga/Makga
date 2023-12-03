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

eAiType ActorAI::Type() const
{
	return m_actor->AIType();
}