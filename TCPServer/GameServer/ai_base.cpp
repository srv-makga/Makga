#include "pch.h"
#include "ai_base.h"

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
}

void BehaviorTree::OnUpdate()
{
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

	switch (_type)
	{
	case eAiType_Aggressive:
		break;
	case eAiType_NonAggressive:
		break;
	case eAiType_Boss:
		break;
	}
}
