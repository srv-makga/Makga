#include "pch.h"
#include "ai_aggressive.h"
#include "actor.h"
#include "actor_ai.h"

AIAggressive::AIAggressive(Actor* _actor)
	: m_actor(_actor)
	, m_root(nullptr)
{
}

void AIAggressive::initialize()
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
				.composite<core::ai::Selector>()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::FindTarget)
						.leaf<ActionNode>(m_actor->AI(), &ActorAI::SetTarget)
					.end()
					.leaf<ActionNode>(m_actor->AI(), &ActorAI::Idle)
				.end()
			.end()
		.end()
		.build();
}

void AIAggressive::terminate(Status s)
{
	switch (s)
	{

	}
}

AIAggressive::Status AIAggressive::update()
{
	m_root->tick();

	return Status::Success;
}
