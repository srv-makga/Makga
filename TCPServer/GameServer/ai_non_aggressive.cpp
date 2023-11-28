#include "pch.h"
#include "ai_non_aggressive.h"

AINonAggressive::AINonAggressive(Actor* _actor)
	: m_actor(_actor)
{
}

AINonAggressive::~AINonAggressive()
{
}

void AINonAggressive::initialize()
{
	// MemSequence
	m_root = core::ai::Builder()
		.composite<core::ai::Sequence>()
			.leaf<IsMoveAbleNode>(m_actor)
			.composite<core::ai::Sequence>()
				.leaf<HasTargetNode>(m_actor)
				.leaf<CanAttackNode>(m_actor, m_actor->Target())
				.leaf<AttackNode>(m_actor, m_actor->Target())
			.end()
			.leaf(MoveNextPos(m_actor))
		.end()
		.build();
}

void AINonAggressive::terminate(Status s)
{
}

AINonAggressive::Status AINonAggressive::update()
{
	m_root->update();
}
