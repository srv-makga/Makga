#include "pch.h"
#include "ai_base.h"
#include "actor.h"

BehaviorTree::BehaviorTree(Actor* _actor)
	: m_actor(_actor)
	, m_node(nullptr)
{
}

BehaviorTree::~BehaviorTree()
{
	delete m_node;
}

void BehaviorTree::Initialize()
{
	ChangeType(m_actor->AIType());
}

void BehaviorTree::OnUpdate()
{
	if (m_node)
	{
		m_node->tick();
	}
}

eAiType BehaviorTree::Type() const
{
	return eAiType();
}

void BehaviorTree::ChangeType(eAiType _type)
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
