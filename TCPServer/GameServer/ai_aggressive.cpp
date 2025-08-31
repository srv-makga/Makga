#include "pch.h"
#include "ai_aggressive.h"
#include "actor.h"
#include "actor_ai.h"

AIAggressive::AIAggressive(std::shared_ptr<Actor> _actor)
	: AI(_actor)
{
}

AIAggressive::~AIAggressive()
{
}

void AIAggressive::initialize()
{
	auto actor_ai = Owner()->GetActorAI();
	
	m_root = core::ai::Builder()
		.composite<core::ai::Selector>()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(actor_ai, &ActorAI::IsDead) // 죽은 상태라면
				.decorator<core::ai::Succeeder>()
					.leaf<ActionNode>(actor_ai, &ActorAI::TryResurrection) // 부활시도
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.leaf<ActionNode>(actor_ai, &ActorAI::HasTarget) // 타겟이 있으면
				.composite<core::ai::Selector>()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(actor_ai ,&ActorAI::IsInsideAttackRange) // 공격 범위 내인지 확인하고
						.leaf<ActionNode>(actor_ai ,&ActorAI::AttackTarget) // 타겟 공격
					.end()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(actor_ai ,&ActorAI::CheckFarSpawnPoistion) // 스폰지역에서 멀리 떨어졌는지 확인하고
						.leaf<ActionNode>(actor_ai ,&ActorAI::MoveToTarget) // 타겟에게 이동
					.end()
					.leaf<ActionNode>(actor_ai ,&ActorAI::ReturnRoutePosition) // 다음 좌표로 이동
				.end()
			.end()
			.composite<core::ai::Sequence>()
				.composite<core::ai::Selector>()
					.composite<core::ai::Sequence>()
						.leaf<ActionNode>(actor_ai ,&ActorAI::FindTarget) // 타겟 찾기
						.leaf<ActionNode>(actor_ai ,&ActorAI::SetTarget) // 타겟 설정
					.end()
					.leaf<ActionNode>(actor_ai ,&ActorAI::Idle) // 방황 상태로 변경
				.end()
			.end()
		.end()
		.build();
}

AIAggressive::Status AIAggressive::update()
{
	if (nullptr == m_root)
	{
		return Status::Failure;
	}

	return m_root->tick();
}
