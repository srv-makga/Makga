#include "pch.h"
#include "ai_non_aggressive.h"
#include "actor.h"
#include "actor_ai.h"

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
		.composite<core::ai::Selector>()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::CheckResurrection)
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::Resurrection)
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::HasTarget)
				.composite<core::ai::Selector>()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::IsInsideAttackRange)
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::AttackTarget)
					.end()
					.leaf<ActionNode>(m_actor->AI(), &ActorAI::Move) // 타켓쪽으로 계속 움직여야 한다
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::NextRoute)
			.end()
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
