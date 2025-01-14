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
	m_root = core::ai::Builder()
		.composite<core::ai::Selector>()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::IsDead)
				.decorator<core::ai::Succeeder>()
					.leaf<ActionNode>(m_actor->AI(), &ActorAI::TryResurrection)
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::HasTarget)
				.composite<core::ai::Selector>()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::IsInsideAttackRange)
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::AttackTarget)
					.end()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::CheckFarSpawnPoistion)
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::MoveToTarget)
					.end()
					.leaf<ActionNode>(m_actor->AI(), &ActorAI::ReturnRoutePosition)
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(m_actor->AI(), &ActorAI::Idle)
			.end()
		.end()
		.build();
}

void AINonAggressive::terminate(Status s)
{
	m_root->terminate(Status::Running);
}

AINonAggressive::Status AINonAggressive::update()
{
	m_root->update();

	return AINonAggressive::Status::Success;
}
