#include "pch.h"
#include "ai_aggressive.h"
#include "actor.h"
#include "actor_ai.h"

AIAggressive::AIAggressive(std::shared_ptr<Actor> _actor)
	: m_actor(_actor)
	, m_root(nullptr)
{
}

AIAggressive::~AIAggressive()
{
	m_actor = nullptr;
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
	default:
		break;
	}
}

AIAggressive::Status AIAggressive::update()
{
	if (nullptr == m_root)
	{
		return Status::Failure;
	}

	return m_root->tick();
}
